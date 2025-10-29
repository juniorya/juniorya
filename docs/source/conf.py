import os
import sys

project = "Delta CNC"
extensions = [
    "breathe"
]

breathe_projects = {"Delta CNC": os.path.join(os.environ.get("SPHINX_BUILD", "${CMAKE_BINARY_DIR}/docs/api"), "html")}
breathe_default_project = "Delta CNC"

templates_path = ["_templates"]
exclude_patterns = []

html_theme = "sphinx_rtd_theme"

latex_engine = "xelatex"
