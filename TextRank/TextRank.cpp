//This code implements the TextRank approach for keyphrase extraction

//Kazi Saidul Hasan, saidul@hlt.utdallas.edu

//09/23/2009

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cerrno>
#include <vector>
#include <regex.h>
#include <time.h>
#include <math.h>
#include <map>
#include <cctype>

using namespace std;

string fileList="";
string goldKeyList="";
string fileDir="";
string goldKeyDir="";
string outputDir="";

int keyCount=0;
int iteration=20;

vector<string> files;//document names
vector<string> keylist;//gold keyphrase file names

long docCount=0;
long docLen=0;
long totalTerms=0;
long windowSize=0;
double ratio=0.0;

vector<string> document;//actual document
vector<string> posTags;//actual document's POS tags

long totalKey=0;
long matched=0;
long predicted=0;

map<string,string> goldKeyMap;//gold keywords' map
vector<string> goldKey;//gold keywords

map<string, map<string,double> > trGraph;
map<string, double> trScore;

map<string,vector<long> > position;//word vs vector of positions where it appears in the doc

void clear()
{
	document.clear();
	posTags.clear();
	goldKeyMap.clear();
	goldKey.clear();	
	
	trGraph.clear();
	trScore.clear();

	position.clear();
	
	docLen=0;
	totalTerms=0;
}

bool isGoodPOS(string pos)
{
	if(pos=="NN" || pos=="NNS" || pos=="NNP" || pos=="NNPS" || pos=="JJ") return true;
	
	return false;
}

string stringToLower(string strToConvert)
{
	string s="";
	
	for(unsigned int i=0;i<strToConvert.length();i++)
    {
    	s+=tolower((int)strToConvert[i]);
    }
   
    return s;
}

void trim(string& str)  
{  
    size_t startpos=str.find_first_not_of(" \t");  
    size_t endpos=str.find_last_not_of(" \t");  
  
    if((string::npos==startpos) || (string::npos==endpos))	str="";  

    else str=str.substr(startpos,endpos-startpos+1);
}

vector<string> tokenize(const string& str, const string& delimiters)
{
	vector<string> tokens;
    	
    string::size_type lastPos=str.find_first_not_of(delimiters,0);
    	
	string::size_type pos=str.find_first_of(delimiters,lastPos);

	while(string::npos!=pos || string::npos!=lastPos)
	{
		tokens.push_back(str.substr(lastPos,pos-lastPos));
	
		lastPos=str.find_first_not_of(delimiters,pos);
	
		pos=str.find_first_of(delimiters,lastPos);
	}

	return tokens;
}

void readFiles(int version, char* file)
{
	char line[500000];
	size_t len=500000;
	char* token;
	
	if(freopen(file,"r",stdin)==NULL)
	{
	   	cout<<"Failed to open the "<<file<<"  file"<<endl<<"Cause : "<<strerror(errno)<<endl;
		exit(-1);
	}
	
	cin.clear();
	
	while(cin.getline(line,len)!=NULL)
  	{
  		string row=line;
  		
		if(version==1)
		{
			files.push_back(row);
			docCount++;
		}
		else if(version==2) keylist.push_back(row);
 	}
  	
  	fclose(stdin);
}

void readTxtFile(int version, char* file, string paperID)
{
	char line[500000];
	size_t len=500000;
	char* token;
	
	if(freopen(file,"r",stdin)==NULL)
	{
	   	cout<<"Failed to open the "<<file<<"  file"<<endl<<"Cause : "<<strerror(errno)<<endl;
		exit(-1);
	}
	
	cin.clear();
	
	while(cin.getline(line,len)!=NULL)
  	{
  		vector<string> row;
  		
  		token=strtok(line," \t\r\n");
  		
		while(token!=NULL)
		{
			row.push_back(token);
			
			token=strtok(NULL," \t\r\n");
		}
		
		if(row.size()>0)
		{			
			for(int i=0;i<row.size();i++)
			{				
				int index=-1;
				
				//each token from the text file is assumed to have the form "word_PoS"
				index=row[i].find_last_of('_');
				
				string word=row[i].substr(0,index);
				string pos=row[i].substr(index+1);
				
				trim(word);
				
				word=stringToLower(word);
				
				if(word.size()>=2 && (word[0]=='<' || word[word.size()-1]=='>')) continue;
				
				if(version==1)
				{					
					//store the original doc
					document.push_back(word);
					posTags.push_back(pos);
					
					//find POS and filter everything except nouns and adjectives
					if(!isGoodPOS(pos))
					{
						docLen++;
						continue;
					}
									
					docLen++;
					
					position[word].push_back(docLen);
					
					if(position[word].size()==1) totalTerms++;
				}				
			}
		}		
  	}
	
  	fclose(stdin);
}

void registerGoldKey(string gkey, int paperID)
{
	trim(gkey);
	gkey=stringToLower(gkey);
	
	if(goldKeyMap[gkey]!=gkey)
	{		
		goldKey.push_back(gkey);
		goldKeyMap[gkey]=gkey;
		
		totalKey++;
	}
}

void readGoldKey(char* file, int& i)
{
	char line[50000];
	size_t len=50000;
	char* token;
	
	if(freopen(file,"r",stdin)==NULL)
	{
	   	cout<<"Failed to open the "<<file<<"  file"<<endl<<"Cause : "<<strerror(errno)<<endl;
		exit(-1);
	}
	
	cin.clear();
	
	while(cin.getline(line,len)!=NULL)
	{
		string row=line;
		
		registerGoldKey(row,i);			
	}  		
  	
  	fclose(stdin);
}

void normalize()
{
	map<string, map<string,double> >::iterator ptr;
	
	for(ptr=trGraph.begin();ptr!=trGraph.end();ptr++)
	{
		map<string,double>::iterator ptr2;
		double sum=0.0;
		
		string word=(*ptr).first;
		
		for(ptr2=trGraph[word].begin();ptr2!=trGraph[word].end();ptr2++)
		{			
			sum+=(*ptr2).second;
		}
		
		for(ptr2=trGraph[word].begin();ptr2!=trGraph[word].end();ptr2++)
		{			
			if(sum)
			{
				trGraph[word][(*ptr2).first]=(*ptr2).second/sum;
			}
			
			else trGraph[word][(*ptr2).first]=0.0;
		}
	}
}

void buildGraph()
{
	for(int i=0;i<document.size();i++)
	{
		string word=document[i];
		
		if(position[word].size())
		{
			int index=i-1;
			double count=windowSize;
			 
			while(index>=0 && count>0)
			{
				string neighbor=document[index];
				
				if(position[neighbor].size())
				{
					//unweighted
					trGraph[word][neighbor]=1.0;
				}
				
				index--;
				count--;
			}
			
			index=i+1;
			count=windowSize;
			
			while(index<document.size() && count>0)
			{
				string neighbor=document[index];
				
				if(position[neighbor].size())
				{
					//unweighted
					trGraph[word][neighbor]=1.0;
				}
				
				index++;
				count--;
			}
		}
	}
	
	normalize();
}

void initialize()
{
	map<string, map<string,double> >::iterator ptr;
	
	for(ptr=trGraph.begin();ptr!=trGraph.end();ptr++)
	{
		if(position[(*ptr).first].size())
		{
			//initial score
			trScore[(*ptr).first]=1.0;
		}
	}
}

void textRank()
{
	initialize();
	
	map<string, double> trScoreTemp;
	
	for(int it=1;it<=iteration;it++)
	{
		map<string,double>::iterator ptr;
	
		for(ptr=trScore.begin();ptr!=trScore.end();ptr++)
		{
			string word=(*ptr).first;
			
			double score=0.0;
			
			map<string,double>::iterator ptr2;
			
			for(ptr2=trGraph[word].begin();ptr2!=trGraph[word].end();ptr2++)
			{
				string neighbor=(*ptr2).first;
				
				if(neighbor!="")
				{
					score+=(trGraph[neighbor][word]*trScore[neighbor]);
				}
			}
			
			score*=0.85;
			
			score+=0.15;

			trScoreTemp[word]=score;			
		}
		
		trScore.clear();
		
		trScore=trScoreTemp;
		
		trScoreTemp.clear();
	}	
}

void replaceLowest(vector<string>& topKey, vector<double>& topKeyVal, string key, double val)
{
	int index=0;
	double minVal=0;
	
	for(int i=0;i<topKeyVal.size();i++)
	{
		if(!i) {index=0; minVal=topKeyVal[i];}
		
		else if(topKeyVal[i]<minVal)
		{
			index=i;
			minVal=topKeyVal[i];
		}
	}
	
	if(val>minVal)
	{
		topKey[index]=key;
		topKeyVal[index]=val;
	}
}

bool isGoldKey(string key)
{		
	for(int i=0;i<goldKey.size();i++)
	{		
		if(goldKey[i]==key)
		{
			return true;
		}
	}
	
	return false;
}

bool isTopKey(vector<string> topKey, string key)
{
	for(int i=0;i<topKey.size();i++)
	{
		if(topKey[i]==key) return true;
	}
	
	return false;
}

void score(char* file)
{
	if(freopen(file,"w",stdout)==NULL)
	{
	   	cout<<"Failed to open the "<<file<<"  file"<<endl<<"Cause : "<<strerror(errno)<<endl;
		exit(-1);
	}
	
	vector<string> topKey;
	vector<double> topKeyVal;
	
	map<string,double>::iterator ptr;
	
	for(ptr=trScore.begin();ptr!=trScore.end();ptr++)
	{
		if((*ptr).second)
		{
			if(topKey.size()<(totalTerms*ratio))
			{
				topKey.push_back((*ptr).first);
				topKeyVal.push_back((*ptr).second);				 
			}
			
			else replaceLowest(topKey,topKeyVal,(*ptr).first,(*ptr).second);
		}
	}
	
	vector<string> finalKey;
	string key="";
	
	for(int i=0;i<document.size();i++)
	{
		if(isTopKey(topKey,document[i]))
		{
			if(key=="") key=document[i];
			else key+=" "+document[i];
		}
		
		else
		{
			if(key!="")
			{
				if(!finalKey.size())
				{
					finalKey.push_back(key);
				}
				
				else
				{
					int flag=0;
					
					for(int j=0;j<finalKey.size();j++)
					{
						if(finalKey[j]==key || finalKey[j]==key+"s" || (key[key.size()-1]=='s' && finalKey[j]==key.substr(0,key.size()-1)))
						{
							flag=1;
							break;
							
						}						
					}
					
					if(!flag)
					{
						finalKey.push_back(key);
					}
				}
			}
			
			key="";
		}
	}
	
	if(key!="")
	{
		if(!finalKey.size())
		{
			finalKey.push_back(key);
		}
		
		else
		{
			int flag=0;
			
			for(int j=0;j<finalKey.size();j++)
			{
				if(finalKey[j]==key || finalKey[j]==key+"s" || (key[key.size()-1]=='s' && finalKey[j]==key.substr(0,key.size()-1)))
				{
					flag=1;
					break;
					
				}						
			}
			
			if(!flag)
			{
				finalKey.push_back(key);
			}
		}
	}
	
	predicted+=finalKey.size();
		
	for(int i=0;i<finalKey.size();i++)
	{
		string pkey=finalKey[i];
		
		cout<<pkey<<endl;
		
		//adding some 'ad-hoc' stemming  
		if(isGoldKey(pkey) || isGoldKey(pkey+"s") || 
		   (pkey[pkey.size()-1]=='s' && isGoldKey(pkey.substr(0,pkey.size()-1))))
		{
			matched++;
		}
	}

	fclose(stdout);
	
	freopen("/dev/tty","w",stdout);//redirecting output back to console
}

void readParams(char* file)
{
	char line[50000];
	size_t len=50000;
	char* token;
	
	int lineNo=0;
	
	if(freopen(file,"r",stdin)==NULL)
	{
	   	cout<<"Failed to open the "<<file<<"  file"<<endl<<"Cause : "<<strerror(errno)<<endl;
		exit(-1);
	}
	
	cin.clear();
	
	while(cin.getline(line,len)!=NULL)
  	{
  		string row=line;
  		lineNo++;
  		
  		vector<string> tokens=tokenize(row,"=\t\r\n");
			
		if(tokens.size()==2)
		{
			if(lineNo==1) fileList=tokens[1];
			else if(lineNo==2) goldKeyList=tokens[1];
			else if(lineNo==3) fileDir=tokens[1];
			else if(lineNo==4) goldKeyDir=tokens[1];
			else if(lineNo==5) outputDir=tokens[1];
			else if(lineNo==6) ratio=atof(tokens[1].c_str());
			else if(lineNo==7) windowSize=atoi(tokens[1].c_str());
		}
 	}
  	
  	fclose(stdin);
}

int main(int argc, char* argv[])
{
	cout<<"Reading params ..."<<endl;
	readParams(argv[1]);
	
	readFiles(1,(char*)fileList.c_str());//document list
    if(goldKeyList!="") readFiles(2,(char*)goldKeyList.c_str());//gold key file list
	
    for(int i=0;i<docCount;i++)
    {    	
    	clear();
    	
		string txt="";
		txt=fileDir+files[i];
	
		cout<<"Processing "<<files[i]<<" ..."<<endl;
		
		readTxtFile(1,(char *)txt.c_str(),files[i]);
		
		string key="";
		
		if(goldKeyDir!="" && i<keylist.size())
		{
			key=goldKeyDir+keylist[i];	    
			readGoldKey((char *)key.c_str(),i);
		}
		
	    buildGraph();
	    
		textRank();
	    
		//score each candidate phrase and output top-scoring phrases
    	score((char*)(outputDir+files[i]+".phrases").c_str());
    }

    cout<<"-------------------------------------------------"<<endl;
  	
	double p=(100.0*matched)/(1.0*predicted);    
	double r=(100.0*matched)/(1.0*totalKey);
	double f=(2.0*p*r)/(p+r);
	
	if(p && r && f)
	{	
		cout<<"Recall = "<<r<<endl;
		cout<<"Precision = "<<p<<endl;
		cout<<"F-score = "<<f<<endl;
	}
	
	cout<<"done"<<endl;
}
