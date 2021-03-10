#include<bits/stdc++.h>
using namespace std;

#define SEC_MEM_SIZE 1000000	//Secondary Memory Size
#define BITS 16
#define MM_ENTRY 1024

struct Record{
	int transactionID;
	int transactionSaleAmount;
	string customerName;
	int itemCategory;
};
struct DirectoryEntry{
	string key;
	int bucketPointer;
};
class GenerateData{
	private:
		int numOfRecords;
		Record *records;
	public:
		GenerateData(int numOfRecords){
			this->numOfRecords = numOfRecords;
			records = new Record[this->numOfRecords];

			srand( (unsigned) time(NULL));
			for(int i = 0;i < this->numOfRecords; i++){
				int rndValue = rand();
				records[i].transactionID = i + 1;
				records[i].transactionSaleAmount = ( rndValue % 500000 ) + 1 ;
				records[i].customerName =  getRandomName(3,rndValue) ; 
				records[i].itemCategory = ( rndValue % 1500 ) + 1;
			}
			
		}
		string getRandomName(int len, int seed){
			string name;
			static const char alphanum[] ="0123456789"
										"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
										"abcdefghijklmnopqrstuvwxyz";

			srand( (unsigned) time(NULL) * seed);
			name.reserve(len);
			for (int i = 0; i < len; ++i) 
        		name += alphanum[rand() % (sizeof(alphanum) - 1)];
			
			return name;
		}
		
		void printRecords(){
			for(int i = 0;i < numOfRecords;i++)
				cout<<records[i].transactionID<<"\t"<<records[i].transactionSaleAmount
				<<"\t"<<records[i].customerName<<"\t"<<records[i].itemCategory<<endl;
			
		}	
		vector<Record> getRecords(){
			vector<Record> recs;
			for(int i = 0;i < numOfRecords;i++) recs.push_back(records[i]);
			return recs;
		}	
};

class BucketADT{
	private:
		bool type; // (0 for Bucket and 1 for Directory
	public:
		BucketADT(bool type){
			this->type = type;
		}
		bool getType(){
			return type;
		}
        virtual int insertRecord(Record record){ return 0;}
		virtual int getLocalDepth(){return 0;}
		virtual void setLocalDepth(int lcDepth){;}
		virtual void setNextBucket(int nextInd){;}
		virtual int getRecords(vector<Record>&rec){return 0;}
		virtual int printRecords(){return 0;}

		virtual int getBucketIndex(string key){return 0;}
		virtual void setPointer(string subKey, int newPointer,int oldPointer, int lcDepth){;}
		virtual bool addEntry(DirectoryEntry dirEntry){ return false;}
		virtual DirectoryEntry getEntry(int indx){return {"0",-1};}
		virtual void updateEntry(int indx){;}


		
};
class Bucket: public BucketADT{
	private:
		int emptySpaces;
		int localDepth;
        int currRecordIndex;
		Record *records;		//Bucket Capacity
		int nextBucket = -1;
    public:
        Bucket(bool type, int bucketSize): BucketADT(type){
            emptySpaces = bucketSize;
            localDepth = 0;
            currRecordIndex = 0;
            records = new Record[bucketSize];
        }
        int insertRecord(Record record){   // TODO
            if(emptySpaces){
                records[currRecordIndex] = record;
                currRecordIndex++;
                emptySpaces--;
                return -2;  //inserted
            }
            return nextBucket;
        }
        int getLocalDepth(){
            return localDepth;
        }
        void setLocalDepth(int lcDepth){
            localDepth = lcDepth;
        }
        void setNextBucket(int nextInd){
            nextBucket = nextInd;
        }
        int getRecords(vector<Record>&rec){
            for(int i=0;i < currRecordIndex;i++){
                rec.push_back(records[i]);
            }
            int nxtPointer = nextBucket;
			/* Update State */
            nextBucket = -1;
			emptySpaces += currRecordIndex;
			currRecordIndex = 0;
			/* Update State */
            return nxtPointer;
        }
		int printRecords(){
			for(int i=0;i < currRecordIndex;i++){
				cout<<records[i].transactionID<<"\t"<<records[i].transactionSaleAmount
				<<"\t"<<records[i].customerName<<"\t"<<records[i].itemCategory<<endl;
            }
			return nextBucket;
		}
};

class Directory: public BucketADT{
	private:
        int emptySpaces;
        int curDirEntryInd;
		DirectoryEntry *dirEntries;
    public:
        Directory(bool type, int bucketSize):BucketADT(type){
            curDirEntryInd = 0;
            emptySpaces = bucketSize;
            dirEntries = new DirectoryEntry[bucketSize];
        }
        int getBucketIndex(string key){
            int bucketPointer = -1;
            for(int i = 0; i < curDirEntryInd; i++){
				//cout<<"key:"<<dirEntries[i].key<<" "<<"Pointer:"<<dirEntries[i].bucketPointer<<endl;
                if(dirEntries[i].key == key){
                    bucketPointer = dirEntries[i].bucketPointer;
					break;
                }
            }
            return bucketPointer;
        }	
		void setPointer(string subKey, int newPointer,int oldPointer, int lcDepth){
			for(int i=0;i< curDirEntryInd ;i++){
				if((dirEntries[i].key).substr(0 ,lcDepth) != subKey && dirEntries[i].bucketPointer == oldPointer){
					dirEntries[i].bucketPointer = newPointer;
				}
			}
		}
		bool addEntry(DirectoryEntry dirEntry){
			if(emptySpaces){
				dirEntries[curDirEntryInd] = dirEntry;
				curDirEntryInd++;
				emptySpaces--;
				return true;
			}
			return false;
		}
		DirectoryEntry getEntry(int indx){
			return dirEntries[indx];
		}
		void updateEntry(int indx){
			dirEntries[indx].key =  dirEntries[indx].key + "0";
		}
		
};

class MainMemory{
    private:
		BucketADT *simSecMem[SEC_MEM_SIZE];
        DirectoryEntry dirEntries[MM_ENTRY];
        int globalDepth;
        int currIndex;
        int currBucketInd;
        int currDirBucketInd;
        int overFlowInd;
        //bool lastDirOverflow;
		//int lastTransId;
		int bucketSize;
		int dirBucketSize;
    public:
        MainMemory(int bucketSize, int dirBucketSize){
			/* Status */
			this->bucketSize = bucketSize;
			this->dirBucketSize = dirBucketSize;
			globalDepth = 0;
            currIndex = 1;
            currBucketInd = 0;
            currDirBucketInd = -1;
            overFlowInd = SEC_MEM_SIZE - SEC_MEM_SIZE/4;
            //lastDirOverflow = false;
            /* Status */
            dirEntries[0] = {"",  currBucketInd};
            simSecMem[currBucketInd] = new Bucket(true, bucketSize);
            currBucketInd++;    // Update Status
        }
		
        string decToBinary(int n) { 
            string bin = "";
            for (int i = BITS-1; i >= 0; i--) { 
                int k = n >> i; 
                if (k & 1) 
                    bin += "1"; 
                else
                    bin += "0"; 
            } 
			cout<<n<<"bin: "<<bin<<endl;
            return bin;
        } 
        string getKey(int transId){
            string bin = decToBinary(transId);
            string key = "";
            if(globalDepth) key = bin.substr(0, globalDepth);	//Changed*****
			//cout<<"Key:"<<key<<endl;
            return key;
        }

        int getBucketIndex(string key){
            int bucketPointer = -1;
            for(int i=0;i<pow(2,globalDepth)&& i < MM_ENTRY;i++){
                if(dirEntries[i].key == key){
                    bucketPointer =  dirEntries[i].bucketPointer;
                    break;
                }
            }
            return bucketPointer;
        }
		DirectoryEntry findDirEntry(int transId){
            DirectoryEntry dirEntry;
            /* find key and bucketPointer */
            string key = getKey(transId);
            int bucketPointer = getBucketIndex(key);
            if(bucketPointer == -1){
                for(int i = SEC_MEM_SIZE-1;i >= currDirBucketInd;i--){
                    bucketPointer = simSecMem[i]->getBucketIndex(key);
					if(bucketPointer != -1)
						break;
                }
            }
            dirEntry.key = key;
            dirEntry.bucketPointer = bucketPointer;
            return dirEntry;
        }
        void expandDir(){
			int i = 0;
			while(i < pow(2, globalDepth)){
				if(currIndex < MM_ENTRY){
					dirEntries[currIndex].bucketPointer = dirEntries[i].bucketPointer;
					dirEntries[currIndex].key =   dirEntries[i].key + "1";
					dirEntries[i].key =  dirEntries[i].key + "0";
					currIndex++;
					i++;
				}
				else if(currIndex >= MM_ENTRY && i < MM_ENTRY){
					if(currDirBucketInd == -1 ){
						currDirBucketInd = SEC_MEM_SIZE - 1 ;
						simSecMem[currDirBucketInd] = new Directory(false, dirBucketSize);
					}
					DirectoryEntry newDirEntry = {  dirEntries[i].key + "1" , dirEntries[i].bucketPointer };
					if(simSecMem[currDirBucketInd]->addEntry(newDirEntry)){
						dirEntries[i].key =  dirEntries[i].key + "0";
						i++;
					}
					else{
						currDirBucketInd--;
						simSecMem[currDirBucketInd] = new Directory(false, dirBucketSize);
					}
				}
				else if(currIndex >= MM_ENTRY && i >= MM_ENTRY){
					int dirBucketInd = SEC_MEM_SIZE -1 - (i-MM_ENTRY)/dirBucketSize;
					int entryInd = (i-MM_ENTRY)%dirBucketSize;
					DirectoryEntry iEntry = simSecMem[dirBucketInd]->getEntry(entryInd);
					DirectoryEntry newDirEntry = { iEntry.key + "1", iEntry.bucketPointer};
					//cout<<"jjjjjj-f:"<<iEntry.key<<" :"<<i<<endl;
					if(simSecMem[currDirBucketInd]->addEntry(newDirEntry)){
						simSecMem[dirBucketInd]->updateEntry(entryInd);
						i++;
					}
					else{
						currDirBucketInd--;
						simSecMem[currDirBucketInd] = new Directory(false, dirBucketSize);
					}

				}
			}
            globalDepth++;  //Update global debth
        }  

		void handleCase1(int bucketPointer, Record record){     
            simSecMem[overFlowInd] = new Bucket(true, bucketSize);
            simSecMem[overFlowInd]->insertRecord(record); //Inserted Record
			cout<<record.transactionID<<":Record Inserted!!"<<endl;
            simSecMem[bucketPointer]->setNextBucket(overFlowInd);
            overFlowInd--;
        }
        void handleCase3(DirectoryEntry dirEntry,Record record){
			int emptyBucketPointer = currBucketInd;
			vector<Record>rec;
			simSecMem[currBucketInd] = new Bucket(true, bucketSize);
			currBucketInd++;
			int nxtPointer = dirEntry.bucketPointer;
			while(nxtPointer != -1){
				nxtPointer = simSecMem[nxtPointer]->getRecords(rec);
			}
			rec.push_back(record);
			int lcDepth = simSecMem[dirEntry.bucketPointer]->getLocalDepth();
			simSecMem[dirEntry.bucketPointer]->setLocalDepth(lcDepth + 1);
			simSecMem[emptyBucketPointer]->setLocalDepth(lcDepth + 1);
			
			// Updated
			string subKey = (dirEntry.key).substr(0 ,lcDepth + 1);
			for(int i=0;i<MM_ENTRY && i < pow(2, globalDepth);i++){
				if((dirEntries[i].key).substr(0 ,lcDepth + 1) != subKey && dirEntries[i].bucketPointer == dirEntry.bucketPointer){
					dirEntries[i].bucketPointer = emptyBucketPointer;
				}
			}
			if(globalDepth > log2(MM_ENTRY)){
				for(int i= SEC_MEM_SIZE-1;i >= currDirBucketInd ;i--){
					simSecMem[i]->setPointer(subKey,emptyBucketPointer,dirEntry.bucketPointer,lcDepth + 1);
				}
			}
			for(int i=0;i<rec.size();i++){
				//cout<<i<<" "<<rec[i].transactionID<<" "<<rec.size()<<endl;
				insertRecord(rec[i],true);
			}
			

        }
		void insertRecord(Record record , bool dirExpanded){
            DirectoryEntry dirEntry = findDirEntry(record.transactionID);
            bool overFlow = false;
            int bucketPointer = dirEntry.bucketPointer;
			int preBucketPointer = bucketPointer;
            while(1){
                bucketPointer = simSecMem[bucketPointer]->insertRecord(record);
                if(bucketPointer == -1){
                    overFlow = true;
                    break;
                }
                else if(bucketPointer == -2){
                    cout<<"Key: "<<dirEntry.key<<"   "<<record.transactionID<<":Record Inserted!!"<<endl;
                    break;
                }
				preBucketPointer = bucketPointer;

            }
            if(overFlow){
                if(globalDepth == simSecMem[dirEntry.bucketPointer]->getLocalDepth()){
                    if(dirExpanded){
						cout<<record.transactionID<<":Overflow Case 1!!"<<endl;
                        handleCase1(preBucketPointer, record);   //Add overFlow Bucket
                        //lastDirOverflow = false;
                    }
                    else{
                        //handleCase2();    //Expand Directory  
						cout<<record.transactionID<<":Overflow Case 2!!"<<endl;
						expandDir();
						//lastDirOverflow = true;
						//lastTransId = record.transactionID;
						insertRecord(record,true);
                    }
                }
                else{
					cout<<record.transactionID<<":Overflow Case 3!!"<<endl;
                    handleCase3(dirEntry, record);      //Add Bucket
                }

            }

        }
		void printDirEntries(){
			
			for(int i=0;i<pow(2,globalDepth);i++){
				
				string key;
				int nxtPointer;
				if(i < MM_ENTRY){
					key = dirEntries[i].key;
					nxtPointer = dirEntries[i].bucketPointer;
				}
				else{
					int dirBucketInd = SEC_MEM_SIZE -1 - (i-MM_ENTRY)/dirBucketSize;
					int entryInd = (i-MM_ENTRY)%dirBucketSize;
					DirectoryEntry iEntry = simSecMem[dirBucketInd]->getEntry(entryInd);
					key = iEntry.key;
					nxtPointer = iEntry.bucketPointer;
				}
				cout<<"Key:"<<key<<"     Bucket:"<<nxtPointer<<endl;
				while(1){
					nxtPointer = simSecMem[nxtPointer]->printRecords();
					if(nxtPointer == -1){
						break;
					}
				}
				cout<<"End------------------------------"<<endl;

			}
		}
		void printOverflowDirectoryEntry(){
			if(pow(2,globalDepth) > MM_ENTRY){
				for(int i=0;i<pow(2,globalDepth)- MM_ENTRY;i++){
					int dirBucketInd = SEC_MEM_SIZE -1 - i/dirBucketSize;
					int entryInd = i%dirBucketSize;
					DirectoryEntry iEntry = simSecMem[dirBucketInd]->getEntry(entryInd);
					if(i%dirBucketSize == 0){
						cout<<"Directory Bucket Index:"<<dirBucketInd<<endl;
					}
					cout<<"---->Entry Key:"<<iEntry.key<<", BucketPointer:"<<iEntry.bucketPointer<<endl;
				}
			}
			else{
				cout<<"*********No Directory OverFlow Bucket!*********"<<endl;
			}
		}

};


void writeToFile(vector<Record>&records,string fileName){
	ofstream myfile;
	myfile.open(fileName);
	if (myfile.is_open())
	{
		for(int i=0;i<records.size();i++){
			myfile<<records[i].transactionID<<"\t"<<records[i].transactionSaleAmount
			<<"\t"<<records[i].customerName<<"\t"<<records[i].itemCategory<<"\n";
			//MM.insertRecord(records[i]);
		}
		myfile.close();
		cout<<"Records Generated!!"<<endl;
	}
  	else 
	  	cout << "Unable to open file\n";
}
vector<Record> readFromFile(string fileName){
	ifstream myfile;
	string line;
	vector<Record> records;
	Record rc;
	myfile.open(fileName);
	if (myfile.is_open())
	{
		while (myfile>>rc.transactionID>>rc.transactionSaleAmount>>rc.customerName>>rc.itemCategory)
		{
			cout<<rc.transactionID<<"\t"<<rc.transactionSaleAmount
			<<"\t"<<rc.customerName<<"\t"<<rc.itemCategory<<endl;
			records.push_back(rc);
		}
		myfile.close();
	}
  	else 
	  	cout << "Unable to open file\n";

	return records;
}


int main(){
	string fileName = "data.txt";
	vector<Record>records;
	bool generateData = true;
	cout<<"For Data Generation(Press 1), Extendible Hash(Press 0):";
	cin>>generateData;
	if(generateData){
		int numOfRecords = 16;	//Default
		cout<<"Enter Number of Records to be Generated:";
		cin>>numOfRecords;
		GenerateData gData(numOfRecords);
		records = gData.getRecords();
		writeToFile(records,fileName);
	}
	else{
		int bucketCap, dirBucketCap; 
		cout<<"Enter Bucket Capacity:";
		cin>>bucketCap;
		cout<<"Enter Directory Bucket Capacity:";
		cin>>dirBucketCap;

		MainMemory MM(bucketCap, dirBucketCap);
		records = readFromFile(fileName);
		for(int i=0;i<records.size();i++){
			//cout<<records[i].transactionID<<"\t"<<records[i].transactionSaleAmount
			//<<"\t"<<records[i].customerName<<"\t"<<records[i].itemCategory<<endl;
			MM.insertRecord(records[i],false);
		}
		char ans;
		cout<<"Do you want to print Directory OverFlow Buckets? Press [Y|n]:";
		cin>>ans;
		if(ans == 'y'|| ans =='Y')
			MM.printOverflowDirectoryEntry();
		cout<<"Do you want to print all records in Buckets? Press [Y|n]:";
		cin>>ans;
		if(ans == 'y'|| ans =='Y')
			MM.printDirEntries();

	}

}
