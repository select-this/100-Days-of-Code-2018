# 100 Days of Code 2018 - Log

##### Day 1: 10/01/2018


**Progress:**
- Set up project, build and unit test environment.

**Thoughts:**
- I'd forgotten how much fun it is to spend an age tweaking your IDE settings to be *just* right

##### Day 2: 11/01/2018

**Progress:**
- Corrected dates (!) and formatting in the markdown files
- Updated project to use C++17 rather than C++14
- Added SDL2 dependency
- Added basic window with two rectangles to represent a depleting bar

**Thoughts:**
- Not as much progress as I'd like, but at least it's a start!
- I've decided this little project will be somewhat a test bed for  getting used to Data Oriented Design principles. To that end, I set up a quick environment in SDL2 since it's something I'm pretty familiar with and doesn't introduce dependency hell, and added a couple of rectangles for my first test.
- The tests *should* be relevant to things that I am either currently working on, or plan to work on in the future, so nothing should be wasted effort.
- Not sure exactly when I got into the habit of placing comment-based separators in my source files, but I find they neatly compartmentalise things for easier reading

##### Day 3: 12/01/2018

**Progress:**
- Added rudimentary timestep to limit logic and display framerate
- Added handling for the x and z keys to control the bar
- Added text to explain controls
- Initial rudimentary depleting bar implementation complete
- Added screenshot directory for showing visual progress

![Day 3 Demo](/screenshot/2018-01-12.gif?raw=true "I promise it looks better than this in reality...")

**Thoughts:**
- Wasted a bit of time forgetting that the erase / remove idiom doesn't work on associative containers
- I have a few concerns about the approach here in terms of indirection - specifically the map and the reference_wrapper key. It would be just as easy to use an unordered_map and provide a hash function (in this case, just returning the integer ID), but I think I'd need to benchmark whether that makes any discernible difference when dealing with a much larger dataset, and whether that would wreck the cache more often.
- There's plenty to improve here - first port of call would be to have a container of rects to be drawn, rather than having an if block around the inner rect (as that's kind of the whole point of the exercise, to reduce branching occurring on every frame).
- Being able to hack out ideas roughly without worrying about much other than the individual problem space is quite liberating. I spend most of my working time deeply entrenched in OO architecture and carefully crafting production code from the outset, so quick 'n' dirty is quite the refreshing change of scenery
- The screen capture is quite jerky! The animation is much smoother in reality - perhaps I can increase the framerate of my capture tool next time...
