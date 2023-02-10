# HangMan Game

This project is a hangman game coded in C language. First, we start the server, then a client connects to the server and chooses a word to make another client connected to the server guess that word. Several games can be run on the server simultaneously.

## Requirements

The C language needs to be compiled. You don't know if you have enough to compile the code?

Linux / macOs :

*(gcc may already be installed if you use one of these systems)*
```bash
> sudo apt-get install gcc
```
Windows :
- Follow [this tutorial](https://www.wikihow.com/Run-C-Program-in-Command-Prompt)

## Install

 First of all, you have to clone the project :

```bash
> git clone https://github.com/Pierre-BISCARAS/HangMan.git
```

Then, go to the HangMan folder

```bash
> cd HangMan
`````
Now, you are in the project.

To play the game, you had to run it.

## Run the project

### Run the server

The server is waiting for two players to connect


**Warning: do not use capital letters or special characters** 

#### Linux / macOs :

You have to start by compiling the file
```bash
> gcc server.c -o server
```
And start the server

```bash
> ./server
```
#### Windows :

In reference to [this tutorial](https://www.wikihow.com/Run-C-Program-in-Command-Prompt), compile and run it.
```bash
> server.exe
```

#### You have the server, now we need our player, the client.

### Run the client

The client needs the server ip and port as arguments.


**Port 5051 is mandatory.**

#### Linux / macOs :

You have to start by compiling the file
```bash
> gcc client.c -o client
```
And start the server

```bash
> ./client 127.0.0.1 5051
```
#### Windows :

In reference to [this tutorial](https://www.wikihow.com/Run-C-Program-in-Command-Prompt), compile and run it.
```bash
> client.exe 127.0.0.1 5051
```
Do the manipulation a second time with another client to be able to play.

## Now enjoy the game !
