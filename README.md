# kafkacow
[![codecov](https://codecov.io/gh/ess-dmsc/kafkacow/branch/master/graph/badge.svg)](https://codecov.io/gh/ess-dmsc/kafkacow)  [![Build Status](https://jenkins.esss.dk/dm/job/ess-dmsc/job/kafkacow/job/master/badge/icon)](https://jenkins.esss.dk/dm/job/ess-dmsc/job/kafkacow/job/master/) [![License (2-Clause BSD)](https://img.shields.io/badge/license-BSD%202--Clause-blue.svg)](https://github.com/ess-dmsc/kafkacow/blob/master/LICENSE)

## Like kafkacat but understands flatbuffers.
The aim of kafkacow is to retrieve messages from kafka and present them in human-readable form(JSON).
Similarly to kafkacat application runs in either __metadata__, __producer__ or __consumer__ mode.
Client runs once and presents/sends data as requested:

```
bin/kafkacow -h

Options:
  -h,--help                   Print this help message and exit
  -C,--consumer               Run the program in the consumer mode.
  -L,--list                   Metadata mode. Show all topics and partitions. If "-t" specified, shows partition offsets.
  -P,--producer               Run program in producer mode.
  -b,--broker TEXT            Hostname or IP of Kafka broker.
  -t,--topic TEXT             Topic to read from/produce to.
  -p,--partition INT          Partition to get messages from.
  -g,--go INT in              How many records back to show from partition "-p". To display range of messages combine with "-o" as lower offset.
  -f,--file FILE              Path to JSON file.
  -o,--offset INT in          Start consuming from an offset. Combine with "-g" to display range of messages with "-o" as lower offset.
  -i,--indentation INT in [0 - 20]
                              Number of spaces used as indentation. Range 0 - 20. 4 by default.
  -a,--all                    Show a list of topics. To be used in "-L" mode.
  -e,--entire                 Show all records of a message(truncated by default).
  -c,--config-file TEXT       Read configuration from an ini file.
  ```
  
  #### Usage example
  Show last `10` messages of `MULTIPART_EVENTS` topic from broker `hinata.isis.cclrc.ac.uk:9092`:
  ```
bin/kafkacow -b hinata.isis.cclrc.ac.uk:9092 -C -g 10 -t MULTIPART_events
  ```
  
  Show list of all topics from broker `hinata.isis.cclrc.ac.uk:9092`:
  ```
 bin/kafkacow -b hinata.isis.cclrc.ac.uk:9092 -L -a
  ```
  
  Starting at offset `1500` show `10` messages from topic `MULTIPART_events` from broker `hinata.isis.cclrc.ac.uk:9092:`
  ```
  bin/kafkacow -b hinata.isis.cclrc.ac.uk:9092 -C -t MULTIPART_events -o 1500 -g 10
  ```
  
  Send data from `/home/user/SendMe.json` to topic `TopicToProduceTo`
  ```
  bin/kafkacow -b hinata.isis.cclrc.ac.uk:9092 -P -t TopicToProduceTo -f /home/user/SendMe.json
  ```
  
  
  ## Install
  
  Kafkacow requires a compiler with std::filesystem or std::experimental::filesystem, such as
  gcc 8,
  clang 7, or
  MSVC 2017.
  
  Kafkacow uses __conan__ to manage dependencies. __Conan__ is recommended to be installed 
  using ```pip``` by running ```sudo pip install conan```. 
  
  If you don't have ```pip``` on your computer please refer to [conan installation guide](https://docs.conan.io/en/latest/installation.html).
  
  
  Add remote repositories to __conan__ using:
  ```conan remote add <local-name> <remote-url>```

 creating a new unique __local name__ for each of the __remote-urls__:
   
   
* https://api.bintray.com/conan/ess-dmsc/conan

* https://api.bintray.com/conan/conan-community/conan
 
* https://api.bintray.com/conan/bincrafters/public-conan

 ## Build
 ```cmake <path-to-source>```

 ```make```
 
 ## Message schemas
Kafkacow uses messages schemas from a repository included as a git subtree.
To update the schemas from the remote repository run
```
git subtree pull --prefix streaming-data-types git@github.com:ess-dmsc/streaming-data-types.git master --squash 
```
 