Test Plan
=========

The test suite consists of:

* Unit tests covering fixed-point utilities, spline sampling, planner timing,
  kinematics (FK/IK/Jacobian), storage integrity, and DDS serialisation.
* Integration tests that emulate an EtherCAT cycle with mock slaves, run the
  ``$SELFTEST`` trajectory, and validate no missed Sync0 events at 8 kHz.
* Documentation builds to ensure Sphinx and Doxygen remain free of warnings.

Run the tests with ``ctest`` after configuring the project with CMake.
