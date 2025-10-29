project = "Delta CNC Controller"
author = "Generated"
extensions = ["breathe"]
html_theme = "alabaster"
import os
breathe_projects = {"api": os.environ.get("DOXYGEN_XML_DIR", "")}
breathe_default_project = "api"
