# sfmlPE
A simple physics engine built using sfml and c++.

A simple physics engine that simulates the motion of balls in a 2D environment.
Several variables can be tweaked mid-simulation, such as friction, restitution, gravity and wind.

The intensity, radius, resolution and threshold of the shader can also be tweaked using the ui.

Controls are:
  Scroll Wheel   :   Tweak options
  Left Click     :   Spawn ball
  F10            :   Spawn wall
  Space          :   Delete all objects
  Z              :   Save state
  X              :   Load state

Momentum, and energy is conserved using real formulas and directions components, applying physics and math concepts.
Each tick, physics are calculated in several substeps, preventing tunneling.
GLSL shaders provide a realtime glow and rainbow effects.
