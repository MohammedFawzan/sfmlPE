<h1 align="center"> sfmlPE</h1>

A simple physics engine built using sfml and c++.

A simple physics engine that simulates the motion of balls in a 2D environment.
Several variables can be tweaked mid-simulation, such as friction, restitution, gravity and wind.

The intensity, radius, resolution and threshold of the shader can also be tweaked using the ui.

Controls are:
```git
*  Scroll Wheel   :   Tweak options & change radius
*  Left Click     :   Spawn ball
*  F10            :   Spawn wall
*  Space          :   Delete all objects
*  Z              :   Save state
*  X              :   Load state
```
Momentum, and energy is conserved using real formulas and directions components, applying physics and math concepts.

Each tick, physics are calculated in several substeps, preventing tunneling.

GLSL shaders provide a realtime glow and rainbow effects.


https://github.com/user-attachments/assets/7e1e06d6-9fad-406a-831e-90fdda145768




https://github.com/user-attachments/assets/83ad2e63-47b7-4355-8c53-323fe7a1cde7



https://github.com/user-attachments/assets/96e638a7-b54f-495b-96cf-5989cb8adf86



https://github.com/user-attachments/assets/226c6773-53ca-47fe-9d66-b8b11c195feb






