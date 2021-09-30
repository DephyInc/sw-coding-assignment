# Coding Assignment

## Aligning Data

At Dephy, a major role of the Software team is to support the Controls team. In this exercise, you get help the Controls team to handle log data.

Spend as much time as you need on this assignment, we are not counting. However, aim to finish it in about 3 hours, we want to be respectful of your time.

### The Problem

For an experiment, data was logged while a test subject was walking. The Controls team needs to analyse the data and compare the readings from gathered during several trials.

In the provided datasets, the right foot was instrumented and touched the ground 6 times (one for each step the subject took).

![](./data_preview.png)

Analizing this data is challenging because the datasets are very large and contain data that is irrelevant to the experiment (i.e. only data columns 0 to 7 are useful in this case)

Also, the files are not logged necesarily at the same frequency and the data from one trial to the next is not aligned (i.e. the subject starts walking at different times). In other works the first accelerometer signal peak doens't happen at the same index.

Finally, the sensor data gets corrupted sometimes.

### Inputs and Outputs

In order to hep the controls team work with this data, you'll need to create a command line tool: `csv_aligner`

The tool can take an arbitrary number of file paths as positional arguments as shown below.

```bash
csv_aligner /PATH/TO/FILE1 /PATH/TO/FILE2 /PATH/TO/FILE3 ...
```

The tool aligns all the data according to the first peak and outputs it into a new CSV file. The data must be labeled so that we know which file it came from.

The tool must also display some useful stats about the data (e.g. number of data points, max/min/average, good vs corrupted data).

### Requirements
1. The tool must be written in C++
2. The code must use at least two classes and have them interact together
3. The tool must be cross-platform (Linux, and Windows or MacOS)
4. Detailed build instructions must be provided so we can build and run your code easilly.

### Evaluation Criteria
1. Does it compile and run?
2. Is the output correct?
3. Does it use appropriate algorithms and data structures.
4. Is the style consistent (e.g. autoformatted)
5. Is it well documented (e.g. doxygen, usage info)
5. Is it fast? More than a few seconds of processing is probably too much.

### Submitting the Solution

In order to submit your solution, fork this repo, create your code and submit a Pull Request once you're ready to be evaluated.
