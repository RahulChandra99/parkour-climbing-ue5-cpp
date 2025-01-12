# Recreating the Climbing Mechanics from The Legends of Zelda

This project is focused on developing advanced parkour and traversal mechanics in Unreal Engine using C++. The goal is to create a seamless, dynamic movement system inspired by parkour-based games like *Assassin's Creed*, *Mirror’s Edge*, and *Dying Light*. The project emphasizes procedural generation of climbing routes and traversal systems to enable characters to fluidly navigate through varied environments, including vertical surfaces, ledges, and obstacles.

## Key Features
- **Procedural Climbing**: Automatically detect and generate climbing paths based on environment geometry.
- **Parkour Movements**: Includes wall running, vaulting, sliding, and free running animations.
- **Traversal System**: Allows fluid movement transitions between ground, walls, and ledges.
- **Animation Blueprint**: Custom procedural animation for seamless player movement, avoiding pre-baked animations for maximum flexibility.
- **Environmental Interaction**: Characters dynamically respond to objects, slopes, and other environmental factors during traversal.
  
## Project Goals
1. **Create a Modular Traversal System**: The system should be reusable and flexible enough to adapt to different environments and gameplay requirements.
2. **Animation Transitions**: Ensure smooth transitions between running, climbing, vaulting, and other traversal states using a procedural approach.
3. **Physics-based Movement**: Integrate physics to make movement feel weighty and responsive, simulating realistic parkour motion.
4. **AI Integration**: Ensure the AI can also navigate the environment using the same traversal mechanics as the player.

## TODOs
- [X] **Climbing System**: 
    - Implement procedural detection of climbable surfaces.
    - Handle character attachment and detachment from surfaces.
    - Add logic for multiple climbing states (idle, moving, transitioning between surfaces).
- [ ] **Wall Running Mechanic**:
    - Detect walls and initiate wall running based on character speed and angle of approach.
    - Integrate seamless transitions from wall running to other parkour actions like vaulting or climbing.
- [ ] **Vaulting and Ledge Grabbing**:
    - Add procedural vaulting over obstacles of varying heights.
    - Implement ledge grabbing with dynamic edge detection.
- [ ] **Custom Animation Blueprint**:
    - Create an animation blueprint that smoothly blends between different parkour states.
    - Ensure procedural animation blending between idle, running, and jumping.
- [ ] **Character Movement Enhancements**:
    - Improve character physics for parkour movements (jumping, sliding, rolling).
    - Add momentum-based calculations for wall running and jumps.
- [ ] **Traversal System Testing**:
    - Stress test in complex environments with different terrain types (urban, nature, etc.).
    - Test in both large and tight spaces to ensure flexibility of the traversal mechanics.
  
## Future Improvements
- [ ] **AI Navigation**: Implement AI traversal capabilities that mimic the player’s parkour movements.
- [ ] **Procedural Animation Refinements**: Continuously optimize animations for better visual quality and performance.
- [ ] **Multiplayer Support**: Extend the traversal system to support multiplayer scenarios with synchronized parkour mechanics.
- [ ] **Customization**: Allow players to customize their movement style and animations (e.g., choose between fast-paced or fluid parkour).


