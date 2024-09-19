
TArray<AActor*>; MyActors;
void PopulateArray(int n)
{
for(int i = 0; i < n; ++i)
{
AActor* ActorToAdd = GetWorld()->SpawnActor<AActor>();
MyActors.Add(ActorToAdd);
}
}
void PrintArray(TArray<AActor*>; Array)
{
for(auto Actor : Array)
{
UE_LOG(LogTemp, Warning, TEXT(“Actor’s name is: %s”), *Actor->GetName());
}
}
void BeginPlay()
{
PopulateArray(100000);
PrintArray(MyActors);
}

Here are the main issues with this code:
- Performance concerns: The PopulateArray function is creating 100,000 actors, which is likely to cause significant performance issues. Spawning this many actors at once could lead to frame rate drops or even crash the game.
- Memory management: There's no code to clean up or destroy the actors once they're no longer needed. This could lead to memory leaks.
- Passing large arrays by value: In the PrintArray function, the array is passed by value instead of by reference. For large arrays, this can be inefficient as it creates a copy of the entire array.
- Global variable usage: MyActors is likely a global or class member variable. It's generally better to avoid global state when possible.
- Lack of error checking: There's no error checking when spawning actors. It's possible that SpawnActor could fail and return nullptr.
- Potential threading issues: If this code is called from multiple threads, it could lead to race conditions as MyActors is modified without any synchronisation.



Features: 

**Jetpack (24 minutes)**

- The character can activate/deactivate the jetpack using a jump button(Spacebar) by holding it.
- The jetpack applies an upward force when active.
- It has a fuel system that depletes while in use and refills when not in use.
- I will add visual effect and proper animation if I have more time.

**Picking up and throwing cubes (72 minutes)**

- The character can pick up cubes within a certain range.
- Picked up cubes attached to the character's hand.
- The character can throw the held cube with a specified force.
- Players need to hold down the E button to pick up the cube and release the button to throw it.
- I’m facing some issues regarding the physics and collision preset when throwing the cube as the cube is blocked by the character’s body so the cube didn’t launch forward.
- I will add proper animation for picking up and throwing the cube if I have more time.

**Simple world interaction mechanics (13 minutes)**

- Added an interact function to the character which is bound to the F button.
- Created a new actor class for interactive objects like doors and switches.
- The character can interact with these objects within a specified range.
- The red cube will change colour to green once the character interacts with it.
- I would like to improve this feature by adding more object variables in c++ so that users just need to assign which object to change and execute the function in the blueprint.

**Punching and damaging cubes (42 minutes)**

- Added a punch function to the character with animation assigned.
- Updated the pick up cube class to include health and damage handling and implement a 3D widget inside the blueprint to display the health bar.
- Cubes can be punched, take damage, and be destroyed.
- Players can do the punch action by clicking the Left Mouse button.
- I would like to improve this feature by adding a combo attack, properly setting up the collision for each body part so that each part can do the damage.

**Double jumps (25 minutes)**

- Implemented a double jump function in the character class.
- Added jump count tracking and a boolean to check if double jump is available.
- Modified the landed function to reset jump count and enable double jump.
- Players need to press the jump button(Spacebar) while in midair to perform the double jump.
- I would like to improve this by adding different animations for midair jump and receive damage if the character jumps/flies too high and lands on the ground.

**Sprints (10 minutes)**

- Implemented a sprint function in the character class.
- Speed multiplier is used to define the new walk speed as run.
- Players can sprint by holding the Shift button and stop sprint when release the button.
- Need more time to adjust the animation blueprint so the movement can be smoother.

**Wall-run (32 minutes)**

- Implemented wall detection and movement logic for wall-running.
- No button needed as the feature is automatically executed when running or jumping besides the walls.
- I would improve this feature if there is a proper animation for wall-run and add stamina required for the wall-run.
