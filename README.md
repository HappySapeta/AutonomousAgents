# AutonomousAgents
A Flocking Simulation Model using Craig Reynold's Algorithms for Steering Behaviors For Autonomous Characters.

Flocking simulations often appear in nature, such as Schools of Fish or Murmurations of Birds, that create an illusion of a massive, sentient being.
Such simulations are often used in video games to add life to the game world.

This project was developed with the aim of mimicking these amazing creations with simple algorithms and making the best use of available resources.

### Development

* Thanks to Craig Reynold's genius, there was no struggle in creating intelligent-looking agents.
The real challenge was making the best use of the resources and simulating at respectable framerates.
Although such complex simulations are better suited for GPUs this project aims at efficiently using the CPU for this task.

* In the first approach, an actor represented each agent, driven by a Behavior Tree.
Although this solution was simple and worked smoothly for a small number of agents, it started showing its limits when the number went above 20.
This was the least cache-friendly approach, and it clearly showed in the profiler in the form of a series of individual actor ticks.

* The first performance improvement was done by uniting the actors and ticking them all under one actor.
This brought the frame time down by a substantial amount, however, it was not nearly enough to simulate 1000 agents at 60 frames per second.

* The profiler helped spot a process that took a major chunk of the frame time. Draw calls.
Each actor possessed a mesh that required a draw call. A thousand actors equals a thousand draw calls.
This led to the replacement of actors with Instanced Static Meshes followed by a huge improvement in performance.

* Before using ISMCs, the simulation relied on overlap tests performed by the physics engine to determine agents in the proximity of another agent.
Even with a very optimized engine, this process of looking up agents is very costly.
ISMCs replaced actors and eliminated the need to perform physics-based tests.

* With overlap tests gone, the first attempt to solve the problem of finding agents was using a brute-force solution.
It compared the distances of each agent against every other agent to filter the ones that lay within the range of influence.
This naive approach resulted in a quadratic time algorithm that exhausted the CPU with just over 100 agents.

* This led to the final step in increasing performance by using a spatial acceleration structure.
For this simulation, a Binary Implicit Grid was used. This acceleration structure divides the simulation space
into equally sized sections and allows a constant time lookup of any section.
  

### Simulating 1000 agents on the CPU at 60 FPS
https://user-images.githubusercontent.com/35849508/216823564-50a420bf-397d-4016-9f15-51fadb3350b8.mp4

### References :
1. [Research Gate: Steering Behaviours For Autonomous Agents](https://www.researchgate.net/publication/2495826_Steering_Behaviors_For_Autonomous_Characters)
2. [The Nature of Code](https://natureofcode.com/)
