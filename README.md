# RMA Hunter

[http://rmahunter.bioinf.me/](http://rmahunter.bioinf.me/)  
[http://rma-hunter.cf:8915/](http://rma-hunter.cf:8915/)

This is RMA Hunter — a web-based tool to systematically analyze and correct 
reference minor alleles in variant calling data. The tool provides a complete 
list of all potentially interesting RMAs that are not called in the sample 
analyzed or found reference homo-/heterozygous, as well as all 
reference-synonymous variants called in the RMA loci. All variants are 
annotated with correct pathogenicity predictions and reference allele 
frequencies.

To start, please upload your VCF file and (for exome sequencing or target 
sequencing assays) a BED-file containing enrichment intervals. To analyze only 
specific genes of interest, please enter a list of genes (separate with comma 
or a newline) in the box provided.

**Please cite the tool as:**  
Barbitoff Y.A., Bezdvornykh I.V., Serebryakova E.A., Glotov A.S., Glotov O.S. 
and Predeus A.V.  
Systematic correction of reference minor alleles in clinical variant calling. 
(2016)


## Installation 

### Node.JS, npm, forever

~~~
curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash -
sudo apt install -y nodejs
sudo apt install npm
npm install forever -g
~~~

### Hunter

~~~
git clone https://github.com/latur/RMA-Hunter.git ./
npm install
gzip -d public/data/sdf_plus_upd.v2.csv.gz
gzip -d public/data/sdf.v2.csv.gz
chmod +x ./exec/*
forever start ./exec/hunter.js 80
~~~
