import textwrap

# -- Project information -------------------------------------------------------

project = "mutiny"
copyright = "2026, Chad Condon"
author = "Chad Condon"
language = "en"

# -- General configuration -----------------------------------------------------

extensions = [
    "breathe",
    "exhale",
    "myst_parser",
    "sphinx.ext.extlinks",
    "sphinx.ext.intersphinx",
    "sphinxcontrib.moderncmakedomain",
]

intersphinx_mapping = {
    "cmake": ("https://cmake.org/cmake/help/latest/", None),
}

# -- Breathe -------------------------------------------------------------------

# Exhale writes XML here (relative to confdir); must match exhale_args below.
breathe_projects = {"mutiny": "./doxyoutput/xml"}
breathe_default_project = "mutiny"
breathe_domain_by_extension = {"hpp": "cpp"}

# -- Exhale --------------------------------------------------------------------

exhale_args = {
    "containmentFolder": "./api",
    "rootFileName": "library_root.rst",
    "rootFileTitle": "API Reference",
    # Strip prefix from reported file paths in the API docs.
    "doxygenStripFromPath": "..",
    "createTreeView": False,
    # Let Exhale drive Doxygen.
    "exhaleExecutesDoxygen": True,
    # Only specify INPUT and extraction settings — Exhale manages all output
    # settings (GENERATE_XML, XML_OUTPUT, GENERATE_HTML, etc.) itself.
    "exhaleDoxygenStdin": textwrap.dedent("""\
        INPUT                = ../include/mutiny
        RECURSIVE            = YES
        # Include C headers so that C-only macros (TEST_GROUP_SETUP,
        # TEST_GROUP_TEARDOWN, CHECK_EQUAL_INT, …) are available via Breathe.
        # Macros shared with the C++ headers (CHECK, FAIL, TEST, …) produce
        # Doxygen "multiple definitions" warnings; these are harmless — the C++
        # versions in Shell.hpp / test.hpp take precedence in the rendered docs.
        FILE_PATTERNS        = *.hpp *.h
        EXTRACT_ALL          = YES
        EXTRACT_PRIVATE      = NO
        EXTRACT_STATIC       = YES
        ENABLE_PREPROCESSING = YES
        MACRO_EXPANSION      = YES
        # Explicitly define capability macros so Doxygen processes the full
        # API surface without relying on compiler-detection macros
        # (e.g. __EXCEPTIONS, __GXX_RTTI) that it doesn't have.
        PREDEFINED           += MUTINY_HAVE_EXCEPTIONS=1 \\
                                MUTINY_HAVE_RTTI=1 \\
                                MUTINY_USE_STD_CPP_LIB=1 \\
                                MUTINY_USE_STD_STRING=0
        WARN_IF_UNDOCUMENTED = NO
        WARN_AS_ERROR        = YES
        QUIET                = YES
        """),
}

# -- GitHub source links -------------------------------------------------------

_github_root = "https://github.com/thetic/mutiny"
_github_branch = "main"

# :source:`filename <path/to/file>` → link into the GitHub tree
extlinks = {
    "source": (f"{_github_root}/tree/{_github_branch}/%s", "%s"),
}

# -- Options for HTML output ---------------------------------------------------

html_theme = "furo"
html_static_path = ["_static"]
html_title = "mutiny"
html_logo = "_static/logo.png"
html_theme_options = {
    "source_repository": _github_root,
    "source_branch": _github_branch,
    "source_directory": "docs/",
    "footer_icons": [
        {
            "name": "GitHub",
            "url": _github_root,
            "html": '<svg stroke="currentColor" fill="currentColor" stroke-width="0" viewBox="0 0 16 16"><path fill-rule="evenodd" d="M8 0C3.58 0 0 3.58 0 8c0 3.54 2.29 6.53 5.47 7.59.4.07.55-.17.55-.38 0-.19-.01-.82-.01-1.49-2.01.37-2.53-.49-2.69-.94-.09-.23-.48-.94-.82-1.13-.28-.15-.68-.52-.01-.53.63-.01 1.08.58 1.23.82.72 1.21 1.87.87 2.33.66.07-.52.28-.87.51-1.07-1.78-.2-3.64-.89-3.64-3.95 0-.87.31-1.59.82-2.15-.08-.2-.36-1.02.08-2.12 0 0 .67-.21 2.2.82.64-.18 1.32-.27 2-.27.68 0 1.36.09 2 .27 1.53-1.04 2.2-.82 2.2-.82.44 1.1.16 1.92.08 2.12.51.56.82 1.27.82 2.15 0 3.07-1.87 3.75-3.65 3.95.29.25.54.73.54 1.48 0 1.07-.01 1.93-.01 2.2 0 .21.15.46.55.38A8.013 8.013 0 0 0 16 8c0-4.42-3.58-8-8-8z"></path></svg>',
            "class": "",
        },
    ],
}

pygments_style = "a11y-high-contrast-light"
pygments_dark_style = "a11y-high-contrast-dark"

# Macros shared between test.h (C API) and test.hpp/Shell.hpp (C++ API) produce
# duplicate c:macro declarations — Sphinx has no cpp:macro domain, so this is a
# known upstream limitation rather than a documentation error.
suppress_warnings = ["duplicate_declaration.c"]
