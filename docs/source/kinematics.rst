Delta Kinematics
================

Delta robot geometry is defined by ``delta_cfg_t`` which holds base and effector
radii, link lengths, and calibration offsets. The forward and inverse solvers use
Q16.16 fixed-point arithmetic to guarantee deterministic execution in the 125 µs
loop.

Singularities are detected by inspecting the determinant of the Jacobian matrix.
When the metric approaches the configured threshold, the planner requests reduced
velocity, acceleration, and jerk to avoid mechanical stress.

The calibration module stores encoder offsets and scale factors, which are
applied by the kinematics layer before feeding setpoints to the CiA-402 control
loop.
