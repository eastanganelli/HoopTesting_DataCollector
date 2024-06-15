# HoopTesting DataCollector

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
  - [Prerequisites](#prerequisites)
  - [Steps](#steps)
- [Configuration](#configuration)
  - [Database Configuration](#database-configuration)
  - [Station Settings](#station-settings)
- [Usage](#usage)
  - [Starting the Application](#starting-the-application)
  - [Collecting Data](#collecting-data)
  - [Visualizing Data](#visualizing-data)
- [Contributing](#contributing)
  - [Forking the Repository](#forking-the-repository)
  - [Creating a New Branch](#creating-a-new-branch)
  - [Making Changes](#making-changes)
  - [Pushing Changes](#pushing-changes)
  - [Opening a Pull Request](#opening-a-pull-request)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Introduction
HoopTesting DataCollector is a software application designed to collect and visualize data for hoop testing systems. This project is developed using C++ and the Qt framework, providing a robust and user-friendly interface for managing and analyzing test data.

## Features
- **Configure Test**: Configure and prepare data test.
- **Data Collection**: Collect data from hoop testing systems in real-time.
- **Data Visualization**: Visualize collected data using various charts and graphs.

## Installation

### Prerequisites
Before installing HoopTesting DataCollector, ensure you have the following prerequisites:
- **CMake**: Version 3.5 or higher.
- **Qt**: Version 6.7 or higher.
- **C++ Compiler**: MSVC 2019.

### Steps
Follow these steps to install HoopTesting DataCollector:

1. **Clone the Repository**:
    
```sh
  git clone https://github.com/yourusername/HoopTesting_DataCollector.git
  cd HoopTesting_DataCollector
```

2. **Create a Build Directory**:
    
```sh
  mkdir build
  cd build
```

3. **Run CMake to Configure the Project**:
    
```sh
  cmake ..
```

4. **Build the Project**:
    
```sh
  make
```

5. **Run the Application**:
    
```sh
  ./DataCollector
```

## Configuration

### Database Configuration
To configure the database, follow these steps:

1. **Open the Application**: Launch the DataCollector application.
2. **Navigate to the Database Menu**: Go to the `Database` menu and select `Configure`.
3. **Enter Database Details**: Fill in the database hostname, port, username, and password.
4. **Save the Configuration**: Click `Save` to store the database settings.

### Serial port Configuration
To configure the database, follow these steps:

1. **Open the Application**: Launch the DataCollector application.
2. **Navigate to the Serial Port Menu**: Go to the `Serial Port` menu and select `Configure`.
3. **Enter Serial port Details**: Fill in the port and baud rate.
4. **Save the Configuration**: Click `Save` to store the serial port settings.


### Station Settings
To set up station settings:

1. **Navigate to Configure**: Go to the `Configure` in the selected Station Tab.
2. **Choose Material and Specification**: Select the material and specification from the dropdown lists.
3. **Add Temperature and Test Time**: Enter the required temperature and test time settings.
4. **Save the Settings**: Click `Save` to apply the station settings.

## Usage

### Starting the Application
To start the application, run the executable `DataCollector` from the build directory:

```sh
./DataCollector
```