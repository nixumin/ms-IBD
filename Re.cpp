/*
 * ReExact.cpp
 *
 *  Created on: 2014年4月24日
 *      Author: yuankai_local
 */

# include <iostream>
# include <fstream>
# include <vector>
# include <sstream>
# include <algorithm>
# include <iomanip>

using namespace std;

long count_ibd(vector<long>p1,vector<long>p2,long cutoff)
{
	vector<long>com;
	int i1(0),i2(0);
	long sum(0),pre(0);
	while(i1<p1.size() && i2<p2.size())
	{
		if(p1[i1]<p2[i2])
		{
			if(p1[i1]-pre>cutoff)
				sum+=p1[i1]-pre;
			pre=p1[i1];
			++i1;
		}
		else if(p1[i1]>p2[i2])
		{
			if(p2[i2]-pre>cutoff)
				sum+=p2[i2]-pre;
			pre=p2[i2];
			++i2;
		}
		else if(i1==p1.size()-1 && i2==p2.size()-1)
		{
			if(p1[i1]-pre>cutoff)
				sum+=p1[i1]-pre;
			pre=p1[i1];
			++i1;
			++i2;
		}
		else
		{
			++i1;
			++i2;
		}
	}
	return sum;
}

int main(int argc,char *argv[])
{
	ifstream fpi;
	ofstream fpibd;
	int npop(0);
	vector<int>pops;
	pops.clear();
	int rep(1);
	long cutoff(0);
	long nsite;
	for(int i=0;i<argc;++i)
	{
		string targv=argv[i];
		if(targv=="-in")
			fpi.open(argv[++i]);
		else if(targv=="-ibd")
			fpibd.open(argv[++i]);
		else if(targv=="-I")
		{
			npop=atoi(argv[++i]);
			int tpop;
			for(int j=0;j<npop;++j)
			{
				tpop=atoi(argv[++i]);
				pops.push_back(tpop);
			}
		}
		else if(targv=="-rep")
			rep=atoi(argv[++i]);
		else if(targv=="-cutoff")
			cutoff=atoi(argv[++i]);
		else if(targv=="-site")
			nsite=atol(argv[++i]);
	}
	vector<vector<long> >pos;
	pos.clear();
	vector<long>tpos;
	string line;
	while(getline(fpi,line))
	{
		if(line=="//")
			break;
	}
	vector<long>ibd;
	ibd.clear();
	vector<long>total;
	total.resize(npop*(npop+1)/2,0);
	int tt;
	for(int i=0;i<rep;++i)
	{
		pos.clear();
		while(getline(fpi,line))
		{
			tpos.clear();
			if(line=="//")
				break;
			else if(line=="")
				continue;
			long bre;
			istringstream cl(line);
			cl>>line;
			while(cl>>bre)
			{
				tpos.push_back(bre);
			}
			tpos.push_back(nsite);
            sort(tpos.begin(),tpos.end());
			pos.push_back(tpos);
		}
		int pre=0;
		for(int j=0;j<npop;++j)
		{
			long sum=0;
			for(int k=1;k<pops[j];++k)
				for(int l=0;l<k;++l)
				{
					sum+=count_ibd(pos[k+pre],pos[l+pre],cutoff);
				}
			ibd.push_back(sum);
			pre+=pops[j];
		}
		int pre1(pops[0]),pre2(0);
		for(int j=1;j<npop;++j)
		{
			pre2=0;
			for(int k=0;k<j;++k)
			{
				long sum(0);
				for(int l=0;l<pops[j];++l)
					for(int m=0;m<pops[k];++m)
					{
						sum+=count_ibd(pos[l+pre1],pos[m+pre2],cutoff);
					}
				ibd.push_back(sum);
				pre2+=pops[k];
			}
			pre1+=pops[j];
		}
	}
	for(int i=1;i<=npop;++i)
		for(int j=i;j<=npop;++j)
			fpibd<<i<<','<<j<<'\t';
	fpibd<<endl;
	for(int i=0;i<rep;++i)
	{
		tt=0;
		int i1((npop+1)*npop/2*i),i2((npop+1)*npop/2*i+npop);
		for(int j=npop-1;j>0;--j)
		{
			fpibd<<ibd[i1]<<'\t';
			total[tt++]+=ibd[i1];
			for(int k=0;k<j;++k)
			{
				fpibd<<ibd[i2]<<'\t';
				total[tt++]+=ibd[i2];
				++i2;
			}
			++i1;
		}
		fpibd<<ibd[i1]<<endl;
		total[tt++]+=ibd[i1];
	}
	for(int i=1;i<=npop;++i)
		for(int j=i;j<=npop;++j)
			cout<<i<<','<<j<<'\t';
	cout<<endl;
	fpibd<<"Average"<<endl;
	cout<<"Average"<<endl;
	for(int i=0;i<npop*(npop+1)/2;++i)
	{
		if(i==npop*(npop+1)/2-1)
		{
			fpibd<<total[i]/rep<<endl;
			cout<<total[i]/rep<<endl;
		}
		else
		{
			fpibd<<total[i]/rep<<'\t';
			cout<<total[i]/rep<<'\t';
		}
	}
	tt=npop-1;
	int tcount(0);
	fpibd<<"Sharing"<<endl;
	cout<<"Sharing"<<endl;
	for(int i=0;i<npop;++i)
	{
		double share;
		share=(double)total[tcount++]/rep/pops[i]/(pops[i]-1)*2/nsite;
		if(i==npop-1)
		{
			fpibd<<setprecision(8)<<setiosflags(ios::fixed)<<share<<endl;
			cout<<setprecision(8)<<setiosflags(ios::fixed)<<share<<endl;
		}
		else
		{
			fpibd<<setprecision(8)<<setiosflags(ios::fixed)<<share<<'\t';
			cout<<setprecision(8)<<setiosflags(ios::fixed)<<share<<'\t';
			for(int j=0;j<tt;++j)
			{
				share=(double)total[tcount++]/rep/pops[i]/pops[i+j+1]/nsite;
				fpibd<<setprecision(8)<<setiosflags(ios::fixed)<<share<<'\t';
				cout<<setprecision(8)<<setiosflags(ios::fixed)<<share<<'\t';
			}
			--tt;
		}
	}
}


