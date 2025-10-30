import os
import sys

project = "Delta CNC"
extensions = [
    "breathe"
]

api_root = os.environ.get("SPHINX_DOXYGEN_HTML", os.path.join('build', 'docs', 'api', 'html'))
breathe_projects = {"Delta CNC": api_root}
breathe_default_project = "Delta CNC"

templates_path = ["_templates"]
exclude_patterns = []

html_theme = "sphinx_rtd_theme"

latex_engine = "xelatex"
