# CPSC/ECE 3220 Project 2
# John McElvenny (jlmcelv)

# Short Description: 
This project is a threading library. Library users are able to create threads to allow concurrent execution. Some features it has: thread creation, joining, yielding. It also has synchronization functions such as locking, waiting, and signaling. The API is modeled off of pthread but implemented using context swapping.

# Design:
Before I wrote any code, I designed how my functions would work in "pseudocode" first. I kept track of what data structures, thread structures, and design patterns I would use beforehand. After designing everything by hand, I organized the code to employ thread structs, joining structs, and lock structs. My view was that the actual threading code could be straightforward and simple if I kept the "ugly" parts out (contexts, pointer island). The first things I did were polish an thoroughly test my data structures (Linked List, List, Queue) so as to not need to think about low-level routines when writing high-level code. Afterwards, I was able to focus on the design of the library. Overall, I think my code is clean, efficient, and understandable.

# Known problems:
At the time of writing, I have an 82/90 on the autograder. All points are lost because I did not put interrupts in the right place. I've submitted a fix but I wont get to see how autograder likes it in time. So there might be some issues with interrupts being disabled when they shouldn't be.

The most obvious problem is what I lost (or did not receive) credit for in my final submission of the project. My library is slow (or not as fast as Dr. Sorber's) so obviously speed is a "known problem" although the library is still usable. My estimate is that speed is not too far off as all of my functions are O(n). Cooperative testing has no known issues. Preemptive testing is indefinite, meaning there are nearly infinite combinations of preemptive scheduling and there may well be a case that causes the library to crash.
