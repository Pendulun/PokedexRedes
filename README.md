# PokedexRedes
This is a practical work for the Computer Networks class at UFMG.

The objective of this work is to develop a client-server application using TCP/IP Sockets in C with \*nix libraries. The server is to simulate a Pokedex, a type of Pokemon encyclopedia for Pokemon trainers, and hold the names of the Pokemon added by the Pokemon trainer, the client side, via known commands. The server must accept and manage connections from a single Client at a time and both need to be in the same network.

## How to run

### Compiling

If the binaries are not present, you must compile the application to generate both the Client's and the Server's binaries. You can do this executing the Makefile with the command:

```
make
```

on a terminal while in the folder of this project.

### Server

First, you need to run the server on a terminal. The Server application requires two arguments: The type of IP address that it will have (IPv6 or IPv4) and a port number to listen for incoming connections.

```
./server <v4|v6> <port number>
```
Example:
```
./server v4 51511
```

### Client

If the server is already executing, you may now run the Client on another terminal. The Client's application requires two arguments: The IP address where the Server is located in IPv4 (quad dotted notation) or IPv6, depending on the <v4|v6> argument passed to the Server application, and the port number where the Server is listening to.

```
./client <IPv\* address> <port number>
```

Example:

```
./client 127.0.0.1 51511
```

## Application Protocol

If the Client is succesfully connected to the Server, it may now send commands to it.

Rules:

- All commands and the Pokemon names must be lower-case.
- The Pokemon names have a limit of 10 characters in length and must only contain lower-case letters or numbers.
- Any special characters like \\, - or * are not allowed.
- If the Client sends an unknown command it will be disconnected from the Server
- The Pokedex is saved for the life-time of the Server, so if a Client updates the Pokedex, disconnects and another Client connects, the Pokedex might already have Pokemon names in it.
- If the Client sends a well known command, but any Pokemon name doesn't follow the rules an "invalid message" will be sent back to the Client
- All messages sent to or by the Server have a limit of 500 bytes

### Add Pokemon

The Server's Pokedex can hold up to 40 Pokemon names. A Client can add up to 4 Pokemons at a time with the same command. Any other Pokemon name sent at the same time will be ignored.

```
add pikachu bulbasaur charmander snorlax
```

If succesfully added, the Server should send back the following message:

```
pikachu added bulbasaur added charmander added snorlax added
```

If a Pokemon already exists in the Pokedex it should send the following message:

```
pikachu already exists
```

If the Pokedex is already full, the Server will send the following message:

```
limit exceeded
```

### Remove Pokemon

One can remove a Pokemon's name at a time from the Pokedex via the following command:

```
remove pikachu
```

The Server should send back the following message:

```
pikachu removed
```

If the Pokemon doesn't exists in the Pokedex:

```
pikachu doensn't exists
```

### Exchange Pokemon

One can exchange a Pokemon for another via the following command:

```
exchange pikachu bulbasaur
```

If succesfull, the Server should send back:

```
pikachu exchanged
```

If the first Pokemon doesn't exists in the Pokedex, the Server will send:

```
pikachu doesn't exists
```

If the second Pokemon already exists, the server will send back:

```
bulbasaur already exists
```

### List Pokemons

One can get the Pokemon's names in the Pokedex via the following command:

```
list
```

The server should return a list with the Pokemons names:

```
pikachu bulbasaur charmander eevee snorlax
```

### Kill Server

A Client might kill the Server, losing the Pokedex informations and disconnecting the Client via the following command:

```
kill
```

### Disconnecting from the Server

A Client can disconnect from the Server, and not kill it, either by closing it's terminal or holding ctrl+c, while on the terminal, to stop execution.
