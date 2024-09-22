# avr-startup

This is an attempt of expressing AVR code in terms of modern C++, it is intentionally using C++ modules.

You will need very recent build tools (clang, cmake, gcc, ninja).

## Known issues

Building with clang fails during linking stage, I don't plan on working to fix this.

## Development

While linking does not work with clang you can still build all the files.
This will help with your LSP if you have configured one.
I made the project create the `compile_commands.json` file so the LSP works.

For reference I use the following command line to build with clang:
```
cmake -GNinja -DCMAKE_CXX_COMPILER=clang++-19 -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

## Building

To build an actual project you have to use gcc, here's my build command:
```
cmake -GNinja -DCMAKE_CXX_COMPILER=avr-g++-14 -DCMAKE_BUILD_TYPE=Release ..
```

## Disclaimer

I prefer `is_empty()` to the syntax `empty()` in `std` on containers.
Since AVR does not have a standard library containers, concepts and other such things will be added with my own twist on things.
