# Coding Assignment

## Aligning Data

At Dephy, a major role of the Software team is to support the Controls team. In this exercise, you get to help the Controls team to handle logged data.

Spend as much time as you need on this assignment. However, aim to finish it in about 3 hours, we want to remain respectful of your time.

### The Problem

For an experiment, we logged data while a test subject was walking. The Controls team needs to analyze the data and compare the readings gathered during several trials.

We instrumented only the right foot for this experiment. Below, you can see how the foot strikes the floor six times (once for each step).

![](./data_preview.png)

Analyzing this data is challenging because the datasets are vast and contain data that is irrelevant to the experiment (i.e. only data columns 0 to 7 are relevant in this case)

Also, the files are not necessarily logged at the same frequency, and the data from one trial to the next is not aligned (i.e. the subject starts walking at different times). In other words, the first accelerometer signal peak doesn't happen at the same index.

Finally, the sensor data gets corrupted sometimes.

### Inputs and Outputs

To help the Controls team examine this data, you'll need to create a command-line tool: `csv_aligner`

The tool can take an arbitrary number of file paths as positional arguments. See below.

```bash
csv_aligner /PATH/TO/FILE1 /PATH/TO/FILE2 /PATH/TO/FILE3 ...
```

The tool aligns all the data according to the first peak and outputs it into a new CSV file. The data must be labelled so that we know from which file it came.

The tool must also display helpful data stats (e.g. the number of data points, max/min/average, good vs corrupted data).

### Requirements
1. The tool must be written in C++
2. The code must use at least two classes and have them interact together
3. The tool must be cross-platform (Linux, and Windows or macOS)
4. Detailed build instructions must be provided, so we can build and run your code easily

### Evaluation Criteria
1. Does it compile and run?
2. Is the output correct?
3. Does it use appropriate algorithms and data structures.
4. Is the style consistent (e.g. autoformatted)
5. Is it well documented (e.g. Doxygen, usage info)
5. Is it fast? More than a few seconds of processing is probably too much.

### Submitting the Solution

To submit your solution, fork this repo, create your code and submit a Pull Request once you're ready to be evaluated.
