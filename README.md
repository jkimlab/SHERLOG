# SHERLOG
**A full homology-based synteny detection program**


Just given a pairwise alignment between two genomes in diverse formats, such as chain, delta, and paf, SHERLOG searches synteny blocks between the genomes.
SHERLOG can find more than one synteny block for a single genomic region if it is related with duplication, and produce the result in a hierarchical manner with two different scales. In addition, SHERLOG does not rely on a reference genome. 

SHERLOG showed good performances for diverse sizes of genomes as well as a wide range of resolutions.

Quick start
----------------
``` 
git clone https://github.com/jkimlab/SHERLOG.git
cd SHERLOG
make
cd toydata/
../sherlog ./params.txt
```


Install SHERLOG
----------------
**System requirement**
   - Linux x64 (Tested in CentOS 6.9, CentOS 6.10, Ubuntu 16.04 and Ubuntu 18.04)  
   - G++ >= 4.9

**Install SHERLOG using source code**
```
git clone https://github.com/jkimlab/SHERLOG.git
cd SHERLOG
make
```

**Using pre-compiled binary file**  

Pre-compiled binary file can be obtained at https://github.com/jkimlab/SHERLOG/releases/tag/v.1.0.0.


**Install SHERLOG using Docker**
```
git clone https://github.com/jkimlab/SHERLOG.git
cd SHERLOG
docker build -t jkimlab/sherlog:latest .
```


Run SHERLOG
----------------
**Prepare a parameter file**  

```
>alignment
[genome1,genome2] [format]  [path_for_input_alignment_file]

>resolution
[value]

>outdir
[path_for_output_directory]
```
\* An example of parameter file is contained in 'toydata' directory.

1. alignment
  - [genome1, genome2]  
  Names of compared genome. SHERLOG assumes the information of the former genome in the input alignment file to be 'genome1' and latter genome to be 'genome2'.
  - [format]  
  A format of input alignment file. Currently, SHERLOG supports chain, delta (nucmer output), paf format.
  - [path_for_input_alignment_file]  
  A path for input alignment file
 
 
 2. resolution
   - [value]  
   A resolution value. We recommend to use a value larger than 1000 as the resolution.
 
 
 3. outdir
   - [path_for_output_directory]  
   A path for working directory. The output files will be generated in the path.


**Run SHERLOG using the parameter file** 

```
sherlog [parameter file]
```



SHERLOG output
----------------
All output files will be generated in the path for output directory set in the parameter file.

**Main output**

 - large_scale_synteny.txt  
  The file containing large-scale and small-scale synteny blocks in a hierarchy.  
  
 \- Format  
 A following example shows a result of large-scale synteny block constructed using two different small-scale synteny blocks and related small-scale synteny blocks in 'large_scale_synteny.txt'.  
  
  ```
  5   Human.chr14:20708973-24428761   ALLPATHS-LG.2:0-3715752 +             # large-scale synteny block
   5.1 Human.chr14:20708973-22392249   ALLPATHS-LG.2:0-1683039 +            # small-scale synteny block
   5.2 Human.chr14:22392960-24428761   ALLPATHS-LG.2:1683796-3715752   +    # small-scale syntney block
  ```

 In the case of example, the forward-oriented large-scale synteny block with ID 5 is searched between 20,708,973-24,428,761 region of human chromsome 14 and 0-3,715,752 of ALLPATHS-LG scaffold 2.
 The large-scale synteny block is constructed by merging two forward-oriented small-scale synteny blocks with ID 5.1 and 5.2.
 
**Supplementary output**

 - small_scale_synteny.txt  
  The file containing small-scale synteny blocks and alignments in a hierarchy. The used format is same with "large_scale_synteny.txt" file.
  
 - segment.txt  
  The file containing alignment information used for synteny block detection.
 
 - edge_seg.txt / edge_seg.conserve.txt  
  The file containing (conserved) DAG of alignments.
 
 - edge_small_scale_synteny.txt / edge_small_scale_synteny.conserve.txt  
  The file containing (conserved) DAG of small-scale synteny blocks.
