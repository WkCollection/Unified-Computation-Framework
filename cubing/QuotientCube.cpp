/*============================================================================
  File:    QuotientCube.cpp

  Summary: The implement of quotient cube algorithm

  Copyright(c) 2007 by DBNC Lab, South China University of Technology, Guangzhou, Guangdong, P.R.C. 

------------------------------------------------------------------------------
  Modification history:
  2007.01.24 Version 1.0 by Jinguo You
============================================================================*/

#include "QuotientCube.h"

QuotientCube::QuotientCube()
{
	dflag = 0;
	uclsNum = 0;
	fps = NULL;

	caNum = 0; //����
}

QuotientCube::~QuotientCube()
{
	if(fps)
	{
		for(int i = 0; i < 2*(dimsNum + 1); i ++)
		{
			fclose(fps[i]);
		}
		delete[] fps;
	}
}

//noting: ePos = bPos + size
int QuotientCube::findCard(int bPos, int ePos, int d) // Cardinality���ҵ��ƣ�bPos����ʼλ�ã�ePos����ֹλ��
{
	int max = 0;
	for(int i = bPos; i < ePos; i ++)//i:�У�d����
	{
		max = data[dflag][i][d] > max ? data[dflag][i][d] : max; //��d�е����ֵ
	}
	return (max + 1);//+1Ϊ����һ���� < �� ��
}

void QuotientCube::Partition(int bPos, int ePos, int d, int card, int *freq)//S card����ά�ȵ�Ԫ��������
{
	int i;
	int *tmpFreq = new int[card];

	//initialize the frequency; ���Կ�����memset
	for(i = 0; i < card; i++)
	{
		freq[i] = 0;
	}

	//caculate the frequency of input data element occurence. ���㱾����ÿ��ֵ���ֵĴ���
    //Note: assuming the elements begin from 1, so freq[0] should be 0.
	//ע������Ԫ�ش�1��ʼ����Ƶ��[0]ӦΪ0.
	for(i = bPos; i < ePos; i++)
		freq[ data[dflag][i][d] ] ++;

	//caculate the begin of every element. ���ڱ����Ѿ�������ôfreq�б���ľ���ÿ��ֵ�ĳ�ʼλ��
	freq[0] = bPos;
	for(i = 1; i < card; i++)
		freq[i] = freq[i] + freq[i - 1];

	//make a copy of freq  ���Կ�����memcpy����vector����
	for(i = 0; i < card; i++)
	{
		tmpFreq[i] = freq[i];
	}

	for(i = bPos; i < ePos; i++)
	{
		data[!dflag][ tmpFreq[ data[dflag][i][d] ] - 1 ] = data[dflag][i];
		//msrdata[!dflag][ tmpFreq[ data[dflag][i][d] ] - 1 ] = msrdata[dflag][i];
		tmpFreq[ data[dflag][i][d] ] --;
	}

	dflag = !dflag;

	delete[] tmpFreq;
}

void QuotientCube::DFS( const int const cl[], int bPos, int ePos, int d )//S ������ȱ���
{
	int i = 0;
	int j = 0;
	int k = 0;
	//int *outputDimData = new int[dimsNum];
	//float *outputMsrData = new float[msrsNum];

	int *ucl = new int[dimsNum]; //S �ȼ����Ͻ�uclΪһ����������
	for ( i = 0; i < dimsNum; i++ )
	{
		ucl[i] = cl[i];
	} // ���Կ��Ǹ���memset����vector<int>
	

	for(i = 0; i < dimsNum; i ++)
	{
		if(cl[i] == ALL) // ��ǰcell�ĵ�i��ά����ALL
		{
			int diff = 0; //� = iά�У�ÿһ��ֵj - ��һ��ֵbPos
			for(j = bPos + 1; j < ePos; j++)
			{
				diff = data[dflag][j][i] - data[dflag][bPos][i];
				if(diff) // �����
				{
					break;
				}
			} // ѭ���������ҵ��׸��ڵ�i��ά�Ȳ���ȵ�Ԫ��
			if(!diff) // �ҵ�����ȵ�
			{
				ucl[i] = data[dflag][bPos][i]; // �Ͻ�
				//outputDimData[i] = ucl[i];
				k ++;
			}
			//else
			//	outputDimData[i] = 0;
		}
		else // ��ǰcell�ĵ�i��ά�Ȳ���all
		{
			//outputDimData[i] = cl[i];
			k ++;
		}
	}


	//prune the cells which have been examined.��֦���Ͻ�����ݵ�Ԫ
	for(j = 0; j < d; j ++)
	{
		if(cl[j] == ALL && ucl[j] != ALL)// ��ucl[]�������Ͻ磬��֦ɾ��
		{
			delete []ucl;
			//delete []outputDimData;
			//delete []outputMsrData;
			return;
		}
	}
/*
	for(i =0; i < dimsNum; i ++)
	{
		cout<<ucl[i]<<" ";
	}
	cout<<endl;
*/

	#ifdef AGGREGATE
	for(i = 0; i < msrsNum; i ++)
	{
		//msrValues[i] = (this->*aggFunLib[aggFunOrder[i]])(msrdata[dflag], bPos, ePos, i);
		outputMsrData[i] = msrValues[i];
	}
	#endif

	int ca = 1;  //the capacity of a class �������
	for (i=0; i < dimsNum; i++) {
		if (cl[i]==ucl[i] && (ePos-bPos)!=1) continue;
		else  ca = -1;
	}
	if (ca == -1) caNum++;

	outputTodimDataFile(k, ucl, ca);//S���������Ͻ磬������
	//outputTomsrDataFile(k, outputMsrData);

	//delete []outputDimData;
	//delete []outputMsrData;

	aggsNumPerLayer[k] ++;
	
	uclsNum ++;
	
	int *tmpucl = new int[dimsNum];
	for ( i = 0; i < dimsNum; i++ )
	{
		tmpucl[i] = ucl[i];
	} // ����ʹ��memcpy����vector
	for(i = d; i < dimsNum; i++)
	{
		if(ucl[i] == ALL)
		{
			int card = findCard(bPos, ePos, i); 
			//�ҵ������е�i��������Ԫ�أ�����Ԫ��ֵ��1��Ҳ���Ǽ����ƣ�������������Ԫ�ص����Ķ���O(n)
			//Does it need to judge the card?
			int *freq = new int[card];
			Partition(bPos, ePos, i, card, freq); // freq�������ÿ��ֵ�ĳ�ʼλ��O(n)

			int tmpdflag = dflag;
			for(j = 0; j < card - 1; j ++)
			{
				if(freq[j + 1] > freq[j])
				{
					ucl[i] = data[dflag][ freq[j] ][i]; // �ҵ����Ͻ�
					DFS(ucl, freq[j], freq[j + 1], i);	// ��i��ά�ȣ����������������������d
				}
				dflag = tmpdflag;
			}
			delete[] freq;
		}
		for ( j =0; j < dimsNum; j++)
		{
			ucl[j] = tmpucl[j];
		} // ����ʹ��memcpy����vector
	
	}
	delete []tmpucl;
	delete []ucl;
	
}

inline int QuotientCube::outputTodimDataFile(int layer, int outputDimData[], int ca)
{
	for (int i=0; i<dimsNum; i++) {
		fprintf(fps[layer], "%d ", outputDimData[i]);
	}
	fprintf(fps[layer], ":(%d)\n", ca);

	return 0;
}

inline int QuotientCube::outputTomsrDataFile(int layer, float outputMsrData[])
{
	if( msrsNum != 0 ) {
		for (int i=0; i<msrsNum; i++) {
			fprintf(fps[dimsNum+1+layer], "%d ", outputMsrData[i]);
		}
		fprintf(fps[layer], "\n");
	}

	return 0;
}

//inline int QuotientCube::outputTodimDataFile(int layer, int outputDimData[])
//{
//	
//	if(fwrite(outputDimData, sizeof(int)*dimsNum, 1, fps[layer]) != 1)
//	{
//		cout<<"write aggregate dimdata error."<<endl;
//		return -1;
//	}
//
//	return 0;
//}
//
//inline int QuotientCube::outputTomsrDataFile(int layer, float outputMsrData[])
//{
//	if( msrsNum != 0 )
//	{
//		if(fwrite(outputMsrData,sizeof(float)*msrsNum,1,fps[dimsNum+1+layer]) != 1)
//		{
//			cout<<"write aggregate msrdata error."<<endl;
//			return -1;
//		}
//	}
//	return 0;
//}

void QuotientCube::preCompute()
{
	int i = 0;
	
	//open aggregate files
	fps = new FILE* [2*(dimsNum + 1)];
	for(i = 0; i < dimsNum + 1; i ++)
	{
		/*string aggDimDataFileName = cubeDir + "\\aggDimData" + itos(i) + ".dat";
		fps[i] = fopen(aggDimDataFileName.c_str(), "wb");*/
		string aggDimDataFileName = cubeDir + "\\aggDimData" + itos(i) + ".txt";
		fps[i] = fopen(aggDimDataFileName.c_str(), "w");
		if(fps[i] == NULL)
		{
			cout<<"can't open aggregate dimData files."<<endl;
			exit(-1);
		}

	}

	for(; i < 2*(dimsNum +1); i++)
	{
		/*string aggMsrDataFileName = cubeDir + "\\aggMsrData" + itos(i-dimsNum-1) + ".dat";
		fps[i] = fopen(aggMsrDataFileName.c_str(),"wb");*/
		string aggMsrDataFileName = cubeDir + "\\aggMsrData" + itos(i-dimsNum-1) + ".txt";
		fps[i] = fopen(aggMsrDataFileName.c_str(),"w");
		if(fps[i] == NULL)
		{
			cout<<"can't open aggregate msrData files."<<endl;
			exit(-1);
		}
	}
	aggsNumPerLayer = new int [dimsNum + 1];
	for(i = 0; i < dimsNum + 1; i ++)
	{
		aggsNumPerLayer[i] = 0;
	}
	
	data[0] = Cubing::data;
	data[1] = new int* [tuplesNum];
	msrdata[0] = Cubing::msrdata;
	//msrdata[1] = new float* [tuplesNum];

	msrValues = new float[msrsNum];

	int *cl = new int[dimsNum];

	for(i = 0; i < dimsNum; i ++)
	{
		cl[i] = ALL;
	}

	DFS(cl, 0, tuplesNum, 0);

	cout<<"the amount of cls of Ca>1 is :"<<caNum<<endl;

	string filename = cubeDir + "\\aggsNumPerLayer.txt";
	FILE *layerFile = fopen( filename.c_str(), "w" );
	if(layerFile == NULL)
	{
		cout<<"can't open aggsNumPerLayer files."<<endl;
		exit(-1);
	}

	for (i=0; i<dimsNum+1; i++) {
		fprintf(layerFile, "%d\n", aggsNumPerLayer[i]);
	}

	/*if(fwrite(aggsNumPerLayer, sizeof(int)*(dimsNum+1), 1, temp) != 1)
	{
		cout<<"write aggsNumPerLayer error."<<endl;
		exit(-1);
	}*/

	fclose(layerFile);
	

	delete[] msrValues;
	delete[] aggsNumPerLayer;
	//delete[] msrdata[1];
	delete[] data[1];
	delete[] cl;

	for(i = 0; i < tuplesNum; i++)
		if(Cubing::data[i]) delete[] Cubing::data[i];
	delete Cubing::data;

	for(i = 0; i < tuplesNum; i++)
		if(Cubing::msrdata[i]) delete[] Cubing::msrdata[i];
	delete Cubing::msrdata;
}

int QuotientCube::getAggsNum()
{
	return uclsNum;
}
