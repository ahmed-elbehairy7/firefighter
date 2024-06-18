# The Firefighting RC Car Project

this is an arduino project for an rc car that can put off fire besides being controlled remotely

## Code

You can find the code pretty printed [here](https://github.com/ahmed-elbehairy7/firefighter/blob/main/code.pdf)

Also, the batch files used can be found [here](https://github.com/ahmed-elbehairy7/firefighter/blob/main/bat.pdf)

## V4 Key Differences

-   **Three flame sensors to put off fire wherever it's detected instead of having only one and detecting only the fire before the car**
-   **A servo responding to the fire detected by the sensors and rotating the according to it instead of being fixed on one direction**
-   **going back and forth in the range of fire to fully put it off. For example, if fire detected from the middle sensor, instead of making the weapon fixed on 90 degrees, it constantly rotates back and forth between 60 and 120 degrees until fire is off, and for the right direction it goes from 120 to 180 continuously. lastly, if fire detected from the left side, it goes back and forth between 0 and 60 degrees until the fire is off**
-   **Using the rx interrupt, the car can update it's movement rapidly right after recieving the command instead of waiting for a round of the servo rotation to complete**
-   **Using self-coded, fully customized servo and motor shield libraries, the size of the program was reduced from 3246 bytes to only 1908 bytes**
-   **The new customized libraries also alowed multiple features to be implemented easly**
-   **The ability to rotate the servo manually and turn the automatic mode on and off (made easy with the new customized libraries)**
-   **The ability to turn the weapon (fan/water pump) on and off using the remote in both directions (made easy with the new customized libraries)**
-   **the new customized servo liberary also allowed fully enabling and disabling the servo**
