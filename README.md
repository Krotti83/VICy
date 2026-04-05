# VICy - Open Cognitive Assist System

## Vision

**VICy** (Virtual Intelligence) is a local-first open cognitive system designed to perceive, 
remember, and act within a structured and ethical framework. It combines a modular architecture 
with an associative memory graph, enabling transparent reasoning, context awareness, and 
adaptive behavior.

Unlike traditional AI systems, **VICy** focuses on clarity, control, and real-world integration. 
It is designed to operate efficiently on local hardware, respect user privacy, and support 
humans and other life forms through responsible and verifiable decision-making.

The project follows a long-term vision of building a system that connects perception (*VICySense*), 
memory (*VICyMemoryGraph*), reasoning (*VICyCore*), ethics (*VICyEthos*), and action (*VICyyAct*) into 
a coherent and extensible platform. 

## Project status

**Planning**

Next phase see development roadmap (version 0.1.x.x)

## Philosophy

* *Virtual Intelligence* instead of *Artificial Intelligence*
* Transparency over black-box behavior
* Local execution (privacy + efficiency)
* Ethical constraints as core system layer
* Inspired by practical engineering (*Donald E. Knuth*, *C inventors*, *hacker culture*, etc.)

## System Architecture

### Core Modules

#### VICyCore (required)

* Decision-making
* Query handling
* Orchestration

#### VICyMemoryGraph (required)

* Concept nodes
* Relations
* Long-term memory (disk)
* Short-term memory (RAM)

#### VICyEthos (important)

* Guard system
* Rule validation
* Value-based behavior control

#### VICyContinuity (required)

* Context persistence across sleep/shutdown
* Snapshot of active state

#### VICySense (later releases)

* Sensor input (temperature, pressure, etc.)
* Signal -> perception mapping

#### VICyAct (later releases)

* Interacting with the world
* Actors (relays, motors, etc.)

## Memory Design

### Short-Term Memory (RAM)

* Active context
* Recent interactions
* Current goals
* Temporary associations

### Long-Term Memory (Disk)

* Concepts
* Definitions
* Learned relations
* Persistent knowledge

### Storage Model

* Content-based hashing (SHA-256)
* Nodes + relations stored on disk
* Indexes for fast lookup

## Data Structures

### Node

* ID (hash)
* Label
* Type
* Confidence
* Metadata

### Relation

* From -> to
* Type (is_a, related_to, etc.)
* Weight/strength

## Query System (v0.1)

### Supported queries

* What is *X*?
* Define *X*
* What is related to *X*?
* How is *X* connected to *Y*?

## Interfaces

### User Interface (via Socket)

* Query handling
* Text-based protocol (initial)

### Control Interface (via Socket)

* status
* suspend
* sleep
* resume
* save memory
* logs

## Lifecycle States

* RUNNING
* IDLE
* SUSPENDING
* SLEEPING
* RESTORING

## Continuity System

Before sleep:

* Save relevant context (not full RAM)

After wake:

* Restore active context
* Resume reasoning

## Development Roadmap

### v0.1.x.x

* Basic MemoryGraph
* Node and relation storage
* Simple CLI queries
* Disk persistence

### Later releases

* Context memory
* Improved query engine
* Control interface
* Sensor integration (*VICySense*)
* Acting with the environment (*VICyAct*)
* Internal state model
* Learning system
* Optimization (*C*/*Assembly*/*FPGA*)
* Advanced reasoning

## Licensing

Core system:

* **GPLv3**

Optional modules/interfaces

* Permissive (**ISC**/**MIT**)

## Required build tools

* GNU make
* *C++* compiler with at least **C++20** support

## Building

```
$ git clone git@github.com:Krotti83/VICy.git
$ cd VICy/src/VICy
$ make
```

## Notes

* Keep system modular
* Avoid over-engineering early
* Build -> test -> refine
* Optimize later

## Acknowledgements

This project was developed with the assistance of **ChatGPT**, used as a tool for 
idea refinement, architectural design, and discussion.
