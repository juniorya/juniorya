import os
import sys

project = 'Delta CNC Platform'
author = 'Automation Team'
release = '1.0'

extensions = [
    'breathe',
]

breathe_projects = {
    'delta_cnc': os.path.join(os.path.dirname(__file__), '..', 'api', 'xml'),
}

breathe_default_project = 'delta_cnc'

templates_path = ['_templates']
exclude_patterns = []

html_theme = 'alabaster'

latex_elements = {
    'papersize': 'a4paper',
    'pointsize': '10pt',
}
