
#*********************************************************************
#
#   MODULE NAME:
#       mod_results.py - mod results handler class
#
#   DESCRIPTION:
#       Provides results collection and file generation
#
#   Copyright 2025 by Nate Lenze
#*********************************************************************

#---------------------------------------------------------------------
#                              IMPORTS
#---------------------------------------------------------------------
import os
import git
from datetime import datetime
import xml.etree.ElementTree as ET

#---------------------------------------------------------------------
#                             CLASSES
#---------------------------------------------------------------------
class ModResults:
    # ================================
    # local variables
    # ================================
    def __init__(self, xml_file_path):
        self.xml_file_path = xml_file_path
        self.date_time = datetime.now()
        self.checksum = (git.Repo(search_parent_directories=True)).head.object.hexsha
        self.test_suites = []

    # ================================
    # publish
    # ================================
    def publish(self):
        self.__publish_results()

    # ================================
    # parse xml
    # ================================
    def parse_xml(self):
        tree = ET.parse(self.xml_file_path)
        root = tree.getroot()
        for testsuite in root.findall('testsuite'):
            suite_name = testsuite.get('name')
            tests = []
            for testcase in testsuite.findall('testcase'):
                tests.append({
                    'name': testcase.get('name'),
                    'file': testcase.get('file'),
                    'line': testcase.get('line'),
                    'status': testcase.get('status'),
                    'result': testcase.get('result'),
                    'time': testcase.get('time'),
                    'classname': testcase.get('classname')
                })
            self.test_suites.append({'name': suite_name, 'tests': tests})

    # ================================
    # publish results
    # ================================
    def __publish_results(self):
        results_file = os.path.join(os.path.dirname(self.xml_file_path), "mod_test_results.html")
        with open(results_file, "w") as f:
            f.write("<!DOCTYPE html>\n")
            f.write("<html>\n")
            f.write("<head>\n")
            f.write("<title>Mod Test Results</title>\n")
            f.write("<style>\n")
            f.write("  body { font-family: Arial, sans-serif; color: #333; line-height: 1.4; }\n")
            f.write("  .container { width: 80%; margin: 0 auto; padding: 20px; border: 1px solid #ccc; border-radius: 10px; }\n")
            f.write("  h1 { text-align: center; color: #444; margin-bottom: 20px; }\n")
            f.write("  h2 { color: #555; border-bottom: 1px solid #eee; padding-bottom: 5px; margin-top: 20px; }\n")
            f.write("  .info-table { width: 100%; border-collapse: collapse; margin-bottom: 15px; font-size: 0.9em; }\n")
            f.write("  .info-table td { border: 1px solid #eee; padding: 5px; text-align: left; }\n")
            f.write("  .info-table td:first-child { font-weight: bold; width: 150px; }\n")
            f.write("  .test-suite-block { border: 1px solid #ddd; margin-bottom: 10px; padding: 10px; border-radius: 5px; }\n")
            f.write("  .test-suite-header { display: flex; justify-content: space-between; align-items: center; margin-bottom: 5px; cursor: pointer; }\n")
            f.write("  .test-suite-header h3 { margin: 0; color: #666; }\n")
            f.write("  .test-case-block { border: 1px solid #eee; margin-bottom: 8px; padding: 8px; border-radius: 5px; background-color: #f8f8f8; display: flex; justify-content: space-between; align-items: center; }\n")
            f.write("  .test-case-name { font-weight: bold; }\n")
            f.write("  .status-pass { color: white; background-color: green; padding: 3px 8px; border-radius: 3px; font-size: 0.9em; }\n")
            f.write("  .status-fail { color: white; background-color: red; padding: 3px 8px; border-radius: 3px; font-size: 0.9em; }\n")
            f.write("  .details { padding-left: 15px; display: none; font-size: 0.9em; }\n")
            f.write("  .overall-pass { color: white; background-color: green; padding: 10px; border-radius: 5px; text-align: center; margin-top: 20px; font-weight: bold; }\n")
            f.write("  .overall-fail { color: white; background-color: red; padding: 10px; border-radius: 5px; text-align: center; margin-top: 20px; font-weight: bold; }\n")
            f.write("</style>\n")
            f.write("</head>\n")
            f.write("<body>\n")
            f.write("<div class='container'>\n")
            f.write("<h1>Module Test Results</h1>\n")
            f.write("<table class='info-table'>\n")
            f.write(f"  <tr><td>FUT sha:</td><td>{self.checksum}</td></tr>\n")
            f.write(f"  <tr><td>Date:</td><td>{self.date_time.ctime()}</td></tr>\n")
            f.write("</table>\n")

            pass_fail = True
            for suite in self.test_suites:
                f.write(f"<div class='test-suite-block'>\n")
                f.write(f"  <div class='test-suite-header' onclick=\"toggleDetails('{suite['name']}')\">\n")
                f.write(f"    <h3>{suite['name']}</h3>\n")
                f.write("  </div>\n")
                f.write(f"  <div id=\"{suite['name']}\" class='details'>\n")
                for test in suite['tests']:
                    result = test['result'] == 'completed'
                    pass_fail = pass_fail and result
                    status_class = "status-pass" if result else "status-fail"
                    status_text = "Pass" if result else "Fail"
                    f.write(f"<div class='test-case-block'>\n")
                    f.write(f"  <span class='test-case-name'>{test['name']}</span>\n")
                    f.write(f"  <span class='{status_class}'>{status_text}</span>\n")
                    f.write(f"</div>\n")
                f.write("  </div>\n")
                f.write("</div>\n")

            f.write("<h2>Overall Result</h2>\n")
            overall_result_class = "overall-pass" if pass_fail else "overall-fail"
            f.write(f"<p class='{overall_result_class}'>Overall Test Result: {'Pass' if pass_fail else 'Fail'}</p>\n")

            f.write("<script>\n")
            f.write("function toggleDetails(id) {\n")
            f.write("  var details = document.getElementById(id);\n")
            f.write("  if (details.style.display === 'none' || details.style.display === '') {\n")
            f.write("    details.style.display = 'block';\n")
            f.write("  } else {\n")
            f.write("    details.style.display = 'none';\n")
            f.write("  }\n")
            f.write("}\n")
            f.write("</script>\n")
            f.write("</div>\n")
            f.write("</body>\n")
            f.write("</html>\n")

if __name__ == '__main__':
    # Note: This script is intended to be run from the root of the project directory.
    # The default path is set accordingly.
    xml_path = os.path.join("results", "mod", "mod_test_results.xml")
    results = ModResults(xml_path)
    results.parse_xml()
    results.publish()
