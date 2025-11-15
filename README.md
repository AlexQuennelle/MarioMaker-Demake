<!-- TODO: Fix README -->

# Mario Maker -1

- Build levels using a variety of obstacles and enemies!
- Load and play saved levels.
- Navigate to the end of the level to win!
- Based on the Mario Maker series and recreated from scratch using C++ and the Raylib library

## Embedding in a website
To embed this game in a website, add the following script tag to the HTML file you want to add it to:

<img width="636" height="179" alt="image" src="https://github.com/user-attachments/assets/c17b598a-a54f-45c7-9ccd-c46d68dc69e4" />

And make sure the following JS function is accessible somewhere:

<img width="494" height="112" alt="image" src="https://github.com/user-attachments/assets/6a5b76f1-fe1e-4768-9a73-e8304bfb17d9" />

Then you can compile the project to WASM using Emscripten, and add the output folder to your website directory. The output index.html file should be your iframe target.
