# Tetris in C

Controls:
    - Move left - a
    - Move right - d
    - Rotate clockwise - e
    - Rotate anticlockwise - q
    - Place down quickly - s

Keeps track of score and returns it upon loss.
To play enter "./tetris ". For more information, enter "./tetris -h".

TODO:
    - Add score saving under names (DONE)
    - Add flags for additional options (DONE)
    - Improve UI (DONE)
    - Add predicted piece position (DONE)
    - Add online

Online will be implemented using a UDP server and client.
Since we will live in an era of firewalls and such, we need a public server we can use. luckily, akamai supplies us with one as always.

clients communicate with each other through the server. much like with OXO online, we will have private and public matches. 

private match options are split between host and client, with the client needing to specify who it is connecting to.

public matches will use lobby theory to link up two users. 

how online will work is as follows, in development steps:
    
    - 1) Two users playing, once either one loses, both games stop.
    - 2) Both players can see all grids. grid data is interchanged between clients for this to happen.
    - 3) Players can influence eachothers grids with bricks by scoring rows.

At this point, we pretty much have modern online tetris.

This will be very hard and take up a lot of my time yippie.




