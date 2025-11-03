Planner and Splines
===================

The planner consists of a look-ahead queue and an S-curve profile generator.
Segments are sourced either from G-code motion blocks or from the spline
library.

The spline module supports cubic Bézier, uniform cubic B-splines, quintic
polynomials with continuous jerk, and NURBS-lite with unit weights. Each spline
is sampled into an arc-length look-up table using adaptive Simpson integration.
The Shin–McKay additive time scaling algorithm ensures that velocity, acceleration,
and jerk limits are respected along the entire curve. When the planner detects
insufficient buffer space, it falls back to trapezoidal profiles while preserving
corner tolerances.

The look-ahead window analyses curvature and feed overrides up to 16 segments
ahead, commanding reduced velocity when approaching soft limits, singularities,
or large curvature changes. The resulting setpoints are emitted either directly
at 8 kHz or at 1–2 kHz accompanied by interpolation coefficients for the motion
module.
