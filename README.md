# Project Title: Chord 


## Team members:

<pre>
   Roll number   Name
1. 2019201049    Gajanan Modi
2. 2019201039    Amit Jindal
3. 2019201067	 Aditya Gupta
4. 2019201082    Anupam Mishra
</pre>

An implementation of the Chord protocol, which efficiently finds the node in a distributed system that has a particular data value.

you may have to use net-tools for commands like ifcongig


# Instructions to run code:
## 1. Download 
      Download library if you do not find string.h in your pc (sometimes that command is ubuntu version specific so we have to google it)
      
## 2. To compile the code: 
```bash
make 
```

## 3. To run
Each machine that will act as a Chord node needs to run
```bash
./ChordClient specify-some-port-number-here
eg.: ./ChordClient 12345
```
      
To create a new Chord ring:
```bash
create_chord
```

To join a Chord ring created by another node (NOTE: Can specify the IP and port of ANY node which is a part of that Chord ring, not necessarily the creator):
```bash
join_chord node-ip-address node-port-number
```

To check who is(are) the current node's successor(s):
```bash
display_successor_list
```

To check who is the current node's predecessor:
```bash
display_predecessor
```

To check the current node's identifier:
```bash
display_node_identifier
```

To view the current node's finger table:
```bash
display_finger_table
```

To insert a new key value pair into the chord ring:
```bash
insert_key put-some-string-key put-some-string-value
```

To display the entries in the current node's hash table:
```bash
display_hash_table
```

To search for a key in the chord ring (and retrieve its value, from whichever node has it):
```bash
search_key put-key-string-here
```
To leave the chord ring and exit the program:
```bash
leave_chord
```
