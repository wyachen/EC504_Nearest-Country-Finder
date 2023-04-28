#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
using namespace std;

#define CITY_NUM 30844
#define pi 3.14159

int root_index = -1;
int Ko = 1;
int K = 5;
double node_distance[200] = {99};
int node_ind[200] = {-1};
int counted_nodes = 0;

struct city {
	int id;
	char name[40];
	char state_id[2];
	double lat; // Note: here store the trasfered latitude, not original latitude
	double lng;
	int left;
	int right;
	int parent;
	int depth; // root depth = 0, even depth means x seperation, odd depth means y seperation
	double dis; // distance 
} c[CITY_NUM] = {0, "", "", 0, 0, -1, -1, -1, -1, 99} , q;

double distance(int ind) // compute the distance between query and index city
{
	double x,y,d;
	x = (c[ind].lng-q.lng)*cos(c[ind].lat+q.lat);
	y = c[ind].lat-q.lat;
	//to simlify it, here do not mutiply R,
	//so remind to mutiply R in the end
	//printf("%f, %f\n",x,y);
	d = sqrt(x*x+y*y);
	c[ind].dis = d;
	return d;
}

/*
void swap(struct city a, struct city b)
{
    struct city tmp;
    tmp.id = a.id;
    strcpy(tmp.name, a.name);
    strcpy(tmp.state_id, a.state_id);
    tmp.lat = a.lat;
    tmp.lng = a.lng;
    tmp.left = a.left;
    tmp.right = a.right;
    tmp.parent = a.parent;
    tmp.depth = a.depth;
    
    a.id = b.id;
    strcpy(a.name, b.name);
    strcpy(a.state_id, b.state_id);
    a.lat = b.lat;
    a.lng = b.lng;
    a.left = b.left;
    a.right = b.right;
    a.parent = b.parent;
    a.depth = b.depth;
    
    b.id = tmp.id;
    strcpy(b.name, tmp.name);
    strcpy(b.state_id, tmp.state_id);
    b.lat = tmp.lat;
    b.lng = tmp.lng;
    b.left = tmp.left;
    b.right = tmp.right;
    b.parent = tmp.parent;
    b.depth = tmp.depth;
}
*/

/*
void swap(struct sort_array a, struct sort_array b)
{
	struct sort_array tmp;
	tmp.id = a.id;
	tmp.v = a.v;
	a.id = b.id;
	a.v = b.v;
	b.id = tmp.id;
	b.v = tmp.v;
}
*/

double lat[CITY_NUM], lng[CITY_NUM] = {0};
int ind[CITY_NUM] = {0};

void swap(int a, int b)
{
	double tmpa,tmpb;
	int tmpi;
	tmpa = lat[a]; lat[a] = lat[b]; lat[b] = tmpa;
	tmpb = lng[a]; lng[a] = lng[b]; lng[b] = tmpb;
	tmpi = ind[a]; ind[a] = ind[b]; ind[b] = tmpi;
}

void quicksort(double a[], int l, int r) // b is used to sort the index
{ 
    int i, j;
    double v;

    if (r > l)
    { 
        v = a[r]; i = l-1; j = r;
        for (;;)
        { 
            while (a[++i] < v) ;
            while (a[--j] > v) ;
            if (i >= j) break;
            swap(i, j);
        }
        swap(i, r);
        quicksort(a, l, i-1);
        quicksort(a, i+1, r);
    }
}

// l, r are the left and right bound of sorting range, p is given to node.parent
// the return value would be given to node.parent.left/right
// -1 means NULL
int build_tree(int l, int r, int p)
{
	int i; // loop
	int half = (l+r)/2;
	int half_index;
	int current_depth;
	if(p == -1)//means that this is the process to find root index
		current_depth = 0;
	else
		current_depth = c[p].depth + 1;
	if(current_depth%2 == 0) // x seperation
		quicksort(lng, l, r);
	else if(current_depth%2 == 1) // y seperation
		quicksort(lat, l, r);
	half_index = ind[half];
	if(p == -1) // mark root index 
		root_index = half_index;
	c[half_index].depth = current_depth;
	c[half_index].parent = p;
	if(l <= half-1)
		c[half_index].left = build_tree(l, half-1, half_index);
	else // already leaf node
		c[half_index].left = -1;
	if(half+1 <= r)
		c[half_index].right = build_tree(half+1, r, half_index);
	else
		c[half_index].right = -1;
	return half_index; // given back to node.parent.left/right
}

// from index, search q down to leaf
// return the final leaf index
int downsearch(int ind)
{
	int cur_i = ind; // the current index which is searched
	int fin_i = ind; // the final index which would be returned
	while(cur_i != -1)
	{
		//printf("%d\n",cur_i);
		fin_i = cur_i;
		if(c[cur_i].depth %2 == 0)
		{
			if(q.lng < c[cur_i].lng)
				cur_i = c[cur_i].left;
			else
				cur_i = c[cur_i].right;
		}
		else
		{
			if(q.lat < c[cur_i].lat)
				cur_i = c[cur_i].left;
			else
				cur_i = c[cur_i].right;
		}
	}
	return fin_i;
} 

double find_k_small()
{
	int i,j,m;
	double nearest_k[9]; // nearest_k[i] is the i+1 th smallest distance
	for (i = 0; i < K; i++)
		nearest_k[i] = 99;
	for (i = 0; i < counted_nodes; i++)
	{
		for (j = 0; j < K; j++)
			if(node_distance[i] < nearest_k[j])
			{
				for (m = K-1; m > j; m--)
					nearest_k[m] = nearest_k[m-1];
				nearest_k[j] = node_distance[i];
				break;
			}
	}
	//cout<<nearest_k[0]<<" "<<nearest_k[1]<<" "<<nearest_k[2]<<endl;
	return nearest_k[K-1];
}

int judge(int cur_node)
{
	int i;
	for(i=0;i<counted_nodes;i++)
		if(node_ind[i]==cur_node)
			return 0;
	return 1;
}


void upsearch(int cur_node)
{
	//printf("%d  %f  %f\n",cur_node, c[cur_node].lat, c[cur_node].lng);
	int brother_node;
	double len; // the length between current node and seperating line
	
	if(judge(cur_node) && find_k_small() > distance(cur_node))
	{	
		// record the current node and distance
		node_distance[counted_nodes] = distance(cur_node);
		node_ind[counted_nodes] = cur_node;
		counted_nodes++;
	
		// record for repeat judge
		c[cur_node].dis = node_distance[counted_nodes-1];
	}
	
	if(distance(c[cur_node].parent) <= node_distance[counted_nodes-1] && judge(c[cur_node].parent))
	{
		node_distance[counted_nodes] = distance(c[cur_node].parent);
		node_ind[counted_nodes] = c[cur_node].parent;
		c[c[cur_node].parent].dis = node_distance[counted_nodes];
		counted_nodes++;
	}
	
	if(c[cur_node].depth %2 == 1)
	{
		//len = fabs(q.lng-c[node_ind[counted_nodes-1]].lng);
		c[CITY_NUM].lat = q.lat;
		c[CITY_NUM].lng = c[cur_node].lng;
		len = distance(CITY_NUM);
	}
	else
	{
		//len = fabs(q.lat-c[node_ind[counted_nodes-1]].lat);
		c[CITY_NUM].lng = q.lng;
		c[CITY_NUM].lat = c[cur_node].lat;
		len = distance(CITY_NUM);
	}
	// find the brother node
	if(c[c[cur_node].parent].left == cur_node)
		brother_node = c[c[cur_node].parent].right;
	else
		brother_node = c[c[cur_node].parent].left;
	//printf("%f %f %d %f\n",len,node_distance[counted_nodes-1],brother_node, c[brother_node].dis);
	// if the shortest radias round do not pass seperation line, or there is no brother node, or the brother has been walked
	if(len > find_k_small() || brother_node == -1 || c[brother_node].dis != 99)
		cur_node = c[cur_node].parent;
	else
		cur_node = downsearch(brother_node);
	// only when it comes to root and it satisfies end conditions, it stops. else, up search
	// end condition: come to root, both two sons are walked/ one walked and another s.t. len < shortest_distance
	if(cur_node == root_index && (len > node_distance[counted_nodes-1] || (c[c[cur_node].left].dis != 99 && c[c[cur_node].right].dis != 99)))
		return; //finished!
	else
		upsearch(cur_node);
}

void search_tree()
{
	int cur_node;
	cur_node = downsearch(root_index);
	upsearch(cur_node);
}

int myatoi(const char *nptr)	// string to int
{
	int result = 0;
	int mark = 1;
	const char *p = nptr;
	while (*nptr!='\0')
	{
		if (*nptr>='0'&&*nptr<='9')
		{
			result *= 10;
			result+=*nptr - 48;
			
			nptr++;
 
		}
		else
		{
			if (nptr == p && *nptr == '-')
				mark = 0;
			break;
		}
	}
	if(mark == 0)
		result = -result;
	return result;
}

int main(int argc, char* argv[])
{
	double latq, lngq; // latitude_query, longitude_query
	int i,j;
  	char str[40];
  	char *ret; //used for deleting _ in city names
  	double tmp;
	ifstream infile;
	ifstream inmodelfile;
  	ofstream outfile;
  	ofstream outmodelfile;
  	
  	//input preloaded model
  	inmodelfile.open("model");
  	if(!inmodelfile){
  	
  	//input city data if there is no model loaded
  	infile.open("city_location.txt");

  	infile >> str;
  	infile >> str;
  	infile >> str;
  	infile >> str;
  	for(i = 0; i < CITY_NUM; i++)
  	{
  		infile >> str;
  		//delete the _ in name, and replace it with space
  		//the original data include _ to seperate words in varaibles
  		ret = strchr(str, '_');
		while(ret != NULL)
		{
			*ret = ' ';
			ret = strchr(str, '_');
		}
  		strcpy(c[i].name,str);
  		infile >> str;
  		strcpy(c[i].state_id,str);
  		infile >> tmp;
  		c[i].lat = tmp*pi/180;
  		lat[i] = c[i].lat;
  		infile >> tmp;
  		c[i].lng = tmp;
  		lng[i] = tmp;
  		c[i].id = i;
  		ind[i] = i;
	}
	
	//there should be a process computing the varaiance to know the root separating direction
	//but according to common sense, the lngtude direction of USA is more sparse, so this process is neglected
	/*
	int xsum,ysum;
	xsum = 0;
	ysum = 0;
	for(i = 0; i < CITY_NUM; i++)
	{
		xsum += 
	}
	*/
	
	//build the tree
	build_tree(0, CITY_NUM-1, -1);
	outmodelfile.open("model");
	outmodelfile << root_index << "\n";
	for(i=0; i<CITY_NUM; i++)
	{
		strcpy(str, c[i].name);
		ret = strchr(str, ' ');
		while(ret != NULL)
		{
			*ret = '_';
			ret = strchr(str, ' ');
		}
		outmodelfile << c[i].id << "    " << str << "    " << c[i].state_id << "    " << c[i].lat << "    " << c[i].lng << "    " << c[i].parent << "    " << c[i].left << "    " << c[i].right << "    " << c[i].depth << endl;
	}
	}
	else // loading preloaded model
	{
		inmodelfile >> root_index;
		for(i=0; i<CITY_NUM; i++)
		{
			inmodelfile >> c[i].id >> str >> c[i].state_id >> c[i].lat >> c[i].lng >> c[i].parent >> c[i].left >> c[i].right >> c[i].depth;
			ret = strchr(str, '_');
			while(ret != NULL)
			{
				*ret = ' ';
				ret = strchr(str, '_');
			}
			strcpy(c[i].name, str);
			c[i].dis = 99;
		}
	}
	/* For test 
	// search
	cin >> latq >> lngq >> Ko;
	if (Ko > K)
		K = Ko;
	//latq = 40; lngq = -70;
	*/ 
	if (argc == 4 && myatoi(argv[3]) >= 1 && myatoi(argv[3]) <= 10)
	{
		latq = myatoi(argv[1]);
		lngq = myatoi(argv[2]);
		Ko = myatoi(argv[3]);
	}
	else
	{
		printf("%d",myatoi(argv[3]));
		printf("Please input latitude, longitude, K in order, 1 <= K <= 10.\nUsing command ./near [lat] [lng] [K]");
		return 0; 
	}
	q.lat = latq*pi/180;
	q.lng = lngq;
	if (Ko > K)
		K = Ko;
	//printf("%d\n",root_index);
	//cout<<c[0].parent<<" "<<c[0].dis<<" "<<c[1].parent<<" "<<c[1].dis<<endl;
	search_tree();
	
	//choose the smallest k distance and sort it
	double dis = find_k_small();
	int s;
	int sort_ind[9];
	double sort_dis[9];
	int sort_index = 0;
	int tmpi;
	double tmpd;
	for(i=0; i<counted_nodes; i++) // choose k small
		if(node_distance[i]<=dis)
		{
			sort_ind[sort_index] = node_ind[i];
			sort_dis[sort_index] = node_distance[i];
			sort_index++;
		}
	for(i=0; i<sort_index; i++)  // simple bubble sort, since its size < 10
		for(j=sort_index-1; j>i; j--)
			if(sort_dis[j] < sort_dis[i])
			{
				tmpd = sort_dis[i];
				sort_dis[i] = sort_dis[j];
				sort_dis[j] = tmpd;
				tmpi = sort_ind[i];
				sort_ind[i] = sort_ind[j];
				sort_ind[j] = tmpi; 
			}
	for(i=0; i<Ko; i++)
	{
		//s = node_ind[i];
		//printf("name: %s, state id: %s, latitude: %f, longitude: %f, distance: %f km\n", c[s].name, c[s].state_id, c[s].lat/pi*180, c[s].lng, c[s].dis*6371);
		s = sort_ind[i];
		printf("name: %s, state id: %s, latitude: %f, longitude: %f, distance: %f km\n", c[s].name, c[s].state_id, c[s].lat/pi*180, c[s].lng, c[s].dis*6371);
	}
	
	char state[10] = {""};
	int knncount[5] = {0};
	int flag = 1;
	int state_ind = 0;
	for(i=0;i<5;i++)
	{
		flag = 1;
		for(j=0;j<10;j=j+2)
			if(state[j] == c[sort_ind[i]].state_id[0] && state[j+1] == c[sort_ind[i]].state_id[1])
			{
				knncount[j/2]++;
				flag = 0;
				break;
			}
		if(flag)
		{
			state[j] = c[sort_ind[i]].state_id[0];
			state[j+1] = c[sort_ind[i]].state_id[1];
		}
	}
			
	
	/*
	double shortest = 99;
	int shortind = -1;
	for(i=0; i<CITY_NUM; i++)
		if (distance(i) < shortest)
		{
			shortest = distance(i);
			shortind = i;
		}	
	printf("name: %s, state id: %s, distance: %f km\n", c[shortind].name, c[shortind].state_id, shortest*6371);
  	*/
	return 0;
} 
