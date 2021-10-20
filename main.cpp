
//  main.cpp
//  SpamFilter

//  Created by Nkhil Narang.
//  Copyright © 2019 Nikhil. All rights reserved.


#include <iostream>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <fstream>
#include <stdio.h>

using namespace std;

#define HAM 0
#define SPAM 1
#define Learn 0
#define Decide 1

//Linkedlist
struct keywordArray{

    int count;
    int classed;
    char Word[20];
    keywordArray *next;

};

//For file read and write
struct wordfile{
    int count;
    int classed;
    char Word[20];
};

class linked_list{

private:
    keywordArray *head,*tail;

public:

    static float TotalSpam, TotalHam, SpamWords, HamWords;

    linked_list(){

        head = NULL;
        tail = NULL;

    }

    keywordArray *begin(){

        return head; //*head
    }

    void add_word(int n, char sen[20], int classed = 2){ //NULL -> 2

        keywordArray *tmp = new keywordArray;
        tmp->count = n;
        tmp->classed = classed;
        strcpy(tmp->Word, sen);
        tmp->next = NULL;

        if(head == NULL){

            head = tmp;
            tail = tmp;
        }

        else{

            tail->next = tmp;
            tail = tail->next;
        }

    }

    void print()
    {
        keywordArray * current;
        current = head;
        while (current != NULL)
        {
            cout << current->Word << endl;
            current = current->next;
        }

    }
  
//for deleting elements of list
    void deleteList()
    {
        keywordArray *tmp = head;
        while (tmp != nullptr)
	    {
			delete(tmp);
		    head = head ->next;
	        tmp = head;
        }

        head = NULL;
        tail = NULL;

    }

    float WordProb(char word[20], int classed){

        keywordArray *tmp = head;

        //To calculate the probability of word being spam/ham.
        float prob=0;
        while(tmp!=NULL)
        {
        	/*Formula:
				P(bodyText | spam) = P(word1 | spam) * P(word2 | spam) * …
				P(bodyText | ham) = P(word1 | ham) * P(word2 | ham) * …
        	*/

            if(strcmp(tmp->Word, word)==0&&tmp->classed == classed){
                if(classed)
                   prob += log((tmp->count+1)/(2*SpamWords+HamWords));
                else
                   prob += log((tmp->count+1)/(2*HamWords+SpamWords));
                break;

            }
            tmp = tmp->next;

        }
        if(prob)
            return prob;
        else
            return 0;

    }

}wordlist, classifiedWordList;

float linked_list::TotalSpam = 0, linked_list::TotalHam = 0, linked_list::HamWords = 0, linked_list::SpamWords = 0;

void removeDuplicates(int mode)
{
    keywordArray *ptr1, *ptr2, *dup;

    if(mode)
        ptr1 = wordlist.begin();

    else
        ptr1 = classifiedWordList.begin();

    /* Pick elements one by one */
    while (ptr1 != NULL && ptr1->next != NULL)
    {
        ptr2 = ptr1;

        /* Compare the picked element with rest
         of the elements */
        while (ptr2->next != NULL)
        {
            /* If duplicate then delete it */
            if (strcmp(ptr1->Word,ptr2->next->Word) == 0 && ptr1->classed == ptr2->next->classed)

            {
                /* sequence of steps is important here */
                ptr1->count+=ptr2->next->count;
                dup = ptr2->next;
                ptr2->next = ptr2->next->next;
                delete(dup);
            }
            else
                ptr2 = ptr2->next;
        }
        ptr1 = ptr1->next;
    }
}

void tokenize(int classed, char sentence[1000]){

    if(classed)
        classifiedWordList.TotalSpam++;

    else
        classifiedWordList.TotalHam++;

    //stopword are words which are commonly used
    char stopWord [23][10] = {"spam","ham","about", "and", "are","com","for","from","how","that","the","this", "was","what","when","where","who","will","with","und","the","www","have"};
    char token[20];

    unsigned long len = strlen(sentence);

    for(int i=0;i<len;i++)
        sentence[i] = tolower(sentence[i]);

    int Flag = 1;

    char *point;

    point = strtok(sentence," ,.-/\{}1234567890!");

    while(point!=NULL){
            strcpy(token,point);

            point = strtok (NULL, " ,.-/\{}1234567890!");

        for(int k=0; k<23;k++)
        {
            if(strcmp(stopWord[k], token)==0||(strlen(token)<3))
            {
                Flag = 0;
                break;
            }
            Flag = 1;
        }

        if(Flag){

            classifiedWordList.add_word(1, token, classed);

            if(classed)
                classifiedWordList.HamWords++;

            else
                classifiedWordList.SpamWords++;
        }
        memset(&token[0], 0, 20);
    }

}

void tokenize(char sentence[1000]){

    char stopWord [23][10] = {"spam","ham","about", "and", "are","com","for","from","how","that","the","this", "was","what","when","where","who","will","with","und","the","www","have"};
    char token[20];

    unsigned long len = strlen(sentence);

    for(int i=0;i<len;i++)
        sentence[i] = tolower(sentence[i]);

    int Flag = 0;

    char *point;

    point = strtok(sentence," ,.-/\{}1234567890!");

    while(point!=NULL){
        strcpy(token,point);
        point = strtok (NULL, " ,.-/\{}1234567890!");
        for(int k=0; k<23;k++)
        {

            if(strcmp(stopWord[k], token)==0||(strlen(token)<3))
            {
                Flag = 0;
                break;
            }

            Flag = 1;
        }

        if(Flag)
            wordlist.add_word(1, token);

        memset(&token[0], 0, 20);

    }
}


int decide()

{
    float PHam = (classifiedWordList.TotalHam/(classifiedWordList.TotalHam+classifiedWordList.TotalSpam));
    float PSpam = (classifiedWordList.TotalSpam/(classifiedWordList.TotalSpam+classifiedWordList.TotalHam));

    float PbodyTextSpam = 0;
    float PbodyTextHam = 0;

    keywordArray *list;

    list = wordlist.begin();

    while(list!=NULL)
    {
        PbodyTextHam += classifiedWordList.WordProb(list->Word,HAM);
        PbodyTextSpam += classifiedWordList.WordProb(list->Word,SPAM);

        list = list->next;
    }  

    float ProbSpam = log(PSpam) + PbodyTextSpam;
    float ProbHam = log(PHam) + PbodyTextHam;

    cout<<"\n\nprobability of text being ham is: "<<ProbHam*100/(ProbHam+ProbSpam)<<"%"<<endl;
    cout<<"probability of text being spam is: "<<ProbSpam*100/(ProbHam+ProbSpam)<<"%"<<endl;
    cout<<"\nThe text is classified as -:";
    //since all probability are negetive (due to log function)
    //we will compare which is more neg
    if(ProbHam<ProbSpam)
        return HAM;

    else
        return SPAM;

}


void writeToFile(){

    ofstream fout("wordlist.dat", ios::binary);

    ofstream set;
    set.open("settings.dat", ios::binary);

    wordfile element;

    set.write((char*)&classifiedWordList.TotalHam, sizeof(classifiedWordList.TotalHam));
    set.write((char*)&classifiedWordList.TotalSpam, sizeof(classifiedWordList.TotalSpam));
    set.write((char*)&classifiedWordList.HamWords, sizeof(classifiedWordList.HamWords));
    set.write((char*)&classifiedWordList.SpamWords, sizeof(classifiedWordList.SpamWords));

    set.close();

    keywordArray *tmp;

    tmp = classifiedWordList.begin();

    while (tmp!=NULL) {

        element.count = tmp->classed;
        element.classed = tmp->classed;

        strcpy(element.Word,tmp->Word);

        //Write to binary file

        fout.write((char*)&element,sizeof(element));
        memset(&element.Word[0], 0, 20);
        tmp = tmp-> next;

    }

}

void ReadfromFile(){

    ifstream fin("wordlist.dat", ios::binary);

    wordfile element;

    ifstream set;
    set.open("settings.dat", ios::binary);
    
    /*Reading the count of words from binary file.*/
    set.read((char*)&classifiedWordList.TotalHam, sizeof(classifiedWordList.TotalHam));
    set.read((char*)&classifiedWordList.TotalSpam, sizeof(classifiedWordList.TotalSpam));
    set.read((char*)&classifiedWordList.HamWords, sizeof(classifiedWordList.HamWords));
    set.read((char*)&classifiedWordList.SpamWords, sizeof(classifiedWordList.SpamWords));
    set.close();

    while (fin) {
        //read from file to element
        fin.read((char*)&element, sizeof(element));

        //Write to linked list
        classifiedWordList.add_word(element.count, element.Word, element.classed);
    }

}

void updatelist(int classed)
{
    keywordArray *tmp;
    tmp = wordlist.begin();

    while(tmp!=NULL)
    {
    	/* Adding words from the wordlist of classification set*/
        classifiedWordList.add_word(tmp->count, tmp->Word, classed);
        tmp = tmp->next;
    }
    removeDuplicates(Learn);
}


int main()
{
    ReadfromFile();
    char label[5];
    char choice = '\0';

    ifstream filein;

    int classed,i=0;
    char filename[30],sec[1000]={"How are you"},ch;
    system("clear");
    cout<<"\t\t\tWELCOME TO SPAM FILTER\n";

    do {
	cout<<"\n\nEnter choice: ";
	cout<<"\nL - learning mode using text file, \nC - Learn with cvs data,\nD - Classify a text file,\nR - remove previous data \nx - Exit program";
	cout<<"\ns - show total data in storage,\nW - Classify a sentence\n:-> ";
    choice = getchar();
	getchar();

        switch (choice) {
		    case 'W':
				cout<<"Type a sentence: \n";
				cin.getline(sec,1000);
	            tokenize(sec);
	            removeDuplicates(Decide);

	            if(decide())
	             {
	                classed = SPAM;
	                cout<<"\nSPAM\n\n"<<endl;
				}

	            else {
	                    classed = HAM;
	                    cout<<"\nHAM\n\n"<<endl;
	                }
			break;	

            case 'L':

                cout<<"\n\nSpecify file name and location with .txt: ";

                cin.getline(filename,30);
				cout<<"Specify File class HAM-0, SPAM-1: ";

                cin>>classed;

                filein.open(filename);

                // if file doesn't open/ or is not present
                if(!filein){
                    cout<<"\n\n\tFile not opened please try again";
                    break;
                }

                filein.getline(sec, 1000);

                tokenize(classed, sec);
                removeDuplicates(Learn);

                filein.close();
                //Function to clear all data in char array of sec.
                memset(&sec[0], 0, 1000);
                break;

            case 'D':

                cout<<"\n\nSpecify file name and location with .txt: ";

                cin.getline(filename,30);
                filein.open(filename);

                // if file doesn't open/ or is not present
                if(!filein){
                    cout<<"File not opened please try again";
                    break;

                }
		
				while(filein){
	                filein.getline(sec, 1000);

	                tokenize(sec);
					//Function to clear all data in char array of sec.
					memset(&sec[0], 0, 1000);
	                removeDuplicates(Decide);
				}

                filein.close();

                if(decide())
                {
                    classed = SPAM;
                    cout<<"\nSPAM\n\n"<<endl;
                }

                else {
                    classed = HAM;
                    cout<<"\nHAM\n\n"<<endl;
                }
				cout<<"Is the result for file "<<filename<<" correct? for yes-y no-n: ";
                cin>>choice;
				getchar();

                if(choice == 'y')
                	wordlist.deleteList();

                else
				{
		    		cout<<"\n\nEnter class of file: ";
                    cin>>classed;
                	updatelist(classed);
				}

                wordlist.deleteList();
                break;

	case 'C':
		cout<<"Enter file name: ";
		cin.getline(filename,30);
		filein.open(filename);
		if(!filein) break;
		cout<<"reading file\n\n";
                while(filein)
		{
		filein.getline(label, 5,',');
		filein.getline(sec, 1000,'\n');

		if(strcmp("ham",label)==0)
			classed = HAM;
		else if(strcmp("spam",label)==0)
			classed = SPAM;
		else continue;
                tokenize(classed, sec);
                removeDuplicates(Learn);
                memset(&sec[0], 0, 1000);
                memset(&label[0], 0, 5);
		cout<<".";
		}
		cout<<".... and done!!";
		filein.close();	
		break;

	case 'x':
		cout<<"\n\n\n\t\t\tThank you!!\n\n";
		break;

	case 'R':
		classifiedWordList.deleteList();
		classifiedWordList.TotalSpam = 0;
		classifiedWordList.TotalHam = 0; 
		classifiedWordList.HamWords = 0;
		classifiedWordList.SpamWords = 0;
		cout<<"All previous data is removed!";
		break;

	case 's':
		cout<<"\nData read till now: ";
		cout<<"\n\nTotal Ham messages = "<<classifiedWordList.TotalHam;
		cout<<"\nTotal Spam messages = "<<classifiedWordList.TotalSpam;
		cout<<"\nTotal Ham Words = "<<classifiedWordList.HamWords;
		cout<<"\nTotal Spam Words = "<<classifiedWordList.SpamWords;		
		break;

     default:
            cout<<"Enter right choice \n";
            break;

        }

    } while (choice!='x');

writeToFile();

}
