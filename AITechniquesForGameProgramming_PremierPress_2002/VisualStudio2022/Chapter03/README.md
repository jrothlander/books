# Chapter 3 - Pathfinder Demo

This is the first genetic algorithm demo and it can be a challange to get your head around what is going on. For me, I work in C# and .Net in my day job, so to really understand the code I had to rewrite it in C#. 

At first the code and approach seemed very odd. But after working through it a few times and fixing issues in my C# version, it starts to click and you start to see how the various functions that randomly select integers and floating point numbers effect the results of the algorithm. I messed up the random functions at first and I could not get the algorithm to find a solution. As I worked back through it to get it to work, I realized what was going on here. I recommend you play around with the random functions, remove them, adjust them, and see what I mean. You can also adjusted the code to use a 3 bit gene, adjust the mutation and crossover rate, as well as the number of chromosomes.

### A few suggestions to help you see what is going on. 
* Create a map with all 0s and set the start and end in various locations.
  * This is remove all of the blocks from the map.
  * Since the map is 15x10, start with an end point (8) in 0,0 and the start (5) in 15,10. There will be nothing to block the paths.
    * Now drop the chromosome count down to say 10 and see what happens, try 50, then 100, etc. Notice what is going on.
* If you set the chromosomes to 150, it will find a solution every time. But the solution will be ugly.
* If you set it to 50, it will take much longer to find a solution, but the solution will be much more optimized. 

* Notice that the paths repeat and you can go back and forth from one tile to another again and again.
  * This is what causes this algorithm to not come up with the most optimized solutions.
  * It never found a solution that only used two strait lines and 25 tiles, which would be the best.
  * An algorithm that never repeats the same tile would be a better design. That's what chapter 2 is about. 
  * In future chapters other approaches and better algorithms will be used.  

### 0's Map and a more Complex Maze 
Here are two maps that I should have added as comments in teh source. The first is a 0s map and the second is a much more complicated maze that is much harder for the algorithm to solve, but it does solve it.

With the 0s map, you can better see how the algorithm is working. The maze map lets you push the algorithm a little harder than the original map.

```
const int CBobsMap::map[MAP_HEIGHT][MAP_WIDTH] =
{
  8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5
};

const int CBobsMap::map[MAP_HEIGHT][MAP_WIDTH] =
{	
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
  8, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1,
  1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 0, 1, 0, 1,
  1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1,
  1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1,
  1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 5,
  1, 0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};
```

### Enhancement I'd like to try
* The algorithm is modeling hermaphroditic sexual reproduction.
  * I don't think it was design to be, but that is effectly what it is doing. I think the intent was just to make it simple. 
  * The genetic variations between the parents and children are limited because of this and the mutation and crossovers rates seem to resolve some of this.
  * However, I am curious if adding male and female genome's would make a different.
    * If we only select Dad genome's from the male population, only select Mom genomes from the female population, would it make any difference?
    * Would it allow us to reduce the mutation and crossover rates?
* Figure out a way to create an X and Y chromosome, so that when the crossover occurs (reproduction) it randomly determines the X and Y chromosome patterns and you get a baby boy or girl.  

### Application 
To move beyond the demo, I wired this up to the enemy ship patterns in a Galaga clone I wrote. The enemy ships use the genetic algorithm not to find the best path, but just to generate random paths for the ship movements. It didn't make for great gameplay, but it is intersting. I think I can tweak the algorithm to limit the movement patterns to patterns that are more arcade like, and I think it would actually be useful. The interesting part of this is that a player could never predict the pattern of the enemy ships, as it would be so dynamic and it would likely never repeat. That is not like the original, where you memorize the pattern and take advantage of it. This would be completely dynamic 100% of the time.   

I also want to wire it up to starfield generation used in the background of the game, just to see what sort of patterns it will come up with. 

I also build a pretty complex PacMan clone with more advanced mazes. If you are not aware, PacMan only has a single maze for all levels. I'd like to wire up the PacMan player to a version of this algorithm to see what it can do. But PacMan's goal is to finish the level, not get through the maze. So it will take some tweaking to think it through. I also want to apply a version of this algorithm to the ghosts and see what sort of patterns it comes up with. My last thought to apply this appliation in games, is that it could be wired up to generate mazes completely dynamic. So every level could be a totally randomly generated maze. That would be pretty complex, as the rules it must follow to create a valid maze would get complex. But it is an interesting thought.

These are very odd use-cases but something I had readly available. I think a better use case would be AI movements between to waypoints in a game. But I'd have to build a new game for that. 
