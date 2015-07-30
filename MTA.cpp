//
// Munieshwar Ramdass (0484772)
// May 4th, 2014
// CS2134
// EC
//

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <list>
#include <algorithm>
#include <functional>
#include <limits>
#include <map>
#include <unordered_map>
#include <stack>
#include <queue>

using namespace std;

//This code is from Prof. Hellerstein and Prof. Frankl
#define _USE_MATH_DEFINES // for C // from http://msdn.microsoft.com/en-us/library/4hwaceh6(v=vs.110).aspx
#include <math.h>

double degrad(double d) {
   return d * M_PI / 180;
}

// Code below is adapted from http://www.movable-type.co.uk/scripts/latlong.html
// FYI: That website has an applet that computes the haversine distance. 
// It also has a link that will show the locations on a map,
// with a line between them.
double haverdist(double lat1, double longit1, double lat2, double longit2)
{
     double r = 6371;
     double dlat = degrad(lat2 - lat1);
     double dlongit = degrad(longit2 - longit1);
     double a = sin(dlat/2)*sin(dlat/2)+
        cos(degrad(lat1))*cos(degrad(lat2))*sin(dlongit/2)*sin(dlongit/2);
     double c = 2*atan2(sqrt(a),sqrt(1-a));
     return r*c;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//		To be more uniform, I would have put most of the code below in the 'private' section of the MTA class
//		But to make grading easier for the TAs, I decided to make it all work in this section
//

//Code from Prof Lisa Hellerstein (very slightly modified)
// CS2134
//
// This is a bare-bones implementation of the single-source unweighted
// shortest paths algorithm 
// It does not have nice classes.

// Assumption: The vertices of the graph are labeled from 0 to n - 1,
// where n is the number of vertices.


const int DEFAULT_VAL =  -1;      // must be less than 0

typedef vector<list<int>> Graph;        // The graph is given in an adjacency list.
// Vertices are indexed from 0 to V-1
// The indices of the vertices adjacent to vertex i
// are in the list Graph[i].
// Graph can be directed or undirected.

struct vertexInf                    // Stores information for a vertex
{                                   
	int dist;  // distance to vertex from the source
	int prev;  // previous node in BFS tree
};

void printpath(int j,  vector<vertexInf>& vinfo, vector<string>& int_to_stop_id)
{
    stack<int> t;

	int current = j;
    while (current != DEFAULT_VAL)
    {
		t.push(current);
		current = vinfo[current].prev;
    }
    while (!t.empty())
    {
		cout << int_to_stop_id[ t.top() ] << " ";
		t.pop();
    }
}

// Modified using code from HW04B solution
void shortestpaths( Graph& g, const string& start, const string& end, map<string, int>& stop_id_to_int, vector<string>& int_to_stop_id )
{
    queue<int> q;             // q is the queue of vertex numbers
	int s(0), e(0);
	s = stop_id_to_int[start];
	e = stop_id_to_int[end];
	
    vector<vertexInf> vertices(g.size());               // stores BFS info for the vertices
	// info for vertex j is in position j
	
    for (int j=0; j < vertices.size(); ++j)                 // Initialize distances and prev values
	{
		vertices[j].dist = DEFAULT_VAL;
		vertices[j].prev = DEFAULT_VAL;
	}

	//s = stop_id_to_int.find(start)->second;
    vertices[s].dist = 0;
	//e = stop_id_to_int.find(end)->second;
    //vertices[e].dist = 0;
	
    q.push(s);
    while  (!q.empty() )
    {
		int v = q.front();
		q.pop();
		for (list<int>::const_iterator w = g[v].begin(); w != g[v].end(); w++)
		{ 
            if (vertices[*w].dist == DEFAULT_VAL)          // distance of *w from source not determined yet
            {
                vertices[*w].dist = vertices[v].dist+1;
                vertices[*w].prev = v;
                q.push(*w);
				if ( *w == e ) //found the shortest path to e!
		        {
                    while ( !q.empty( ) ) q.pop( );
        		}
				
            }
		}
    }
	cout << "From " << int_to_stop_id[s] << " to " << int_to_stop_id[e] << endl; 
	cout << "Distance:\t" << ( vertices[e].dist / 3 ) << " stops apart." << endl;
	cout << "Please note that stops are grouped in 3's\n"
		<< "\tand so the list shown below is in fact three times\n"
		<< "\tlonger than the given distance.\n"
		<< "\tNote that there are the original, north, and south\n"
		<< "\tversions of the same stop." << endl;
	cout << "Shortest Path:\t"; 
	printpath(e,vertices, int_to_stop_id); 
	cout << endl;
}


class MTA {

	class trainStopData { // structure given
	public:
		trainStopData( const string& id, const string& name, double lat, double lon )
			: stop_id( id ), stop_name( name ), stop_lat( lat ), stop_lon( lon ) {}

		string get_id( ) const { return stop_id; }
		string get_stop_name( ) const { return stop_name; }
		double get_latitude( ) const { return stop_lat; }
		double get_longitude( ) const { return stop_lon; }

	private:
		string stop_id;		// id of train stop (1st token)
		string stop_name;	// name of station (4th token)
		double stop_lat;	// latitude of train stop location
		double stop_lon;	// longitude of train stop location
	};

public:
	MTA( const string& train_data, const string& transfers )
		: active( false ), train_data( train_data ), transfers( transfers ) {
			bool status = info_collect();
			active = status;
			if ( active ) { menu(); }
	}

	void menu() {
		if ( active ) {
			vector<trainStopData>::iterator itrStart;
			vector<trainStopData>::iterator itrEnd;
			int user_choice, counter( 0 );
			string key;
			double lat, lon, d;
			printTrainStopInfo printObject;
			cout << endl;

			while ( true ) {
				itrStart = data.begin();
				itrEnd = data.end(); // one pass the last element

				cout << "////////////////////////////////////////////////////////////"
					<< "//\n//\n"
					<< "//\t\tWELCOME TO MTA INFO CENTER!\n"
					<< "//\n//\n" << endl;

				cout << "How can we help you? Enter the number that fits you situation\n"
					<< "1 - Print Route Information\n"
					<< "2 - Print Station Information\n"
					<< "3 - Print Stations within a Range\n"
					<< "4 - Print Closest Stop\n"
					<< "5 - Print All Transfers\n"
					<< "6 - Enter Start and End Stop IDs to find Shortest Path\n"
					<< "7 - Print Transfer of Specific Stop\n"
					<< "8 - Enter Start and End Coordinates to find Shortest Path\n"
					<< "0 - Exit Menu\n" << endl;

				cin >> user_choice;
				while ( cin.fail() && !(cin.eof() || cin.bad()) ) {
					cout << "Your choice was not in range [0,8] - Try again:" << endl;
					cin.clear();
					cin.ignore( numeric_limits<std::streamsize>::max(),'\n' );
					cin >> user_choice;
				}

				switch ( user_choice ) {
				case 1: {
					cout << "Enter Letter or Number to search:\t";
					cin >> key;
					transform( key.begin(), key.end(), key.begin(), toupper );
					isStopOnRoute stop( key.front() );
					counter = perform_if( itrStart, itrEnd, stop, printObject );
					if ( counter == 0 )
						cout << "No results.";

					cout << endl;
					break;
						}
				case 2: {
					cout << "Enter stop ID to search:\t";
					cin >> key;
					transform( key.begin(), key.end(), key.begin(), toupper );
					isSubwayStop stop( key );
					counter = perform_if( itrStart, itrEnd, stop, printObject );
					if ( counter == 0 )
						cout << "No stop found on specified route.";

					cout << endl;
					break;
						}
				case 3: {
					cout << "Enter EXACT Latitude and longitude of current stop:\n";
					cout << "Latitude: ";
					cin >> lat;
					cout << "Longitude: ";
					cin >> lon;
					cout << "Now enter how far you'd like to search:\t";
					cin >> d;
					isSubwayStopNearX stop( lat, lon, d );
					counter = sort_stations( lat, lon, itrStart, itrEnd, stop, printObject ); // perform_if was here
					//counter = perform_if( itrStart, itrEnd, stop, printObject ); // Unsorted distances
					if ( counter == 0 )
						cout << "There are no stops within the\n\tspecified distance of specified coordinates.";

					cout << endl;
					break;
						}
				case 4: {
					cout << "Enter Latitude and Longitude of your stop:\n";
					cout << "Latitude: ";
					cin >> lat;							// type double; declared before switch statement
					cout << "longitude: ";
					cin >> lon;							// type double; declared before switch statement
					closest_stop stop( lat, lon );
					vector<trainStopData>::iterator closest = select_item( itrStart, itrEnd, stop ); // returns iterator
					cout << "Best match of location closest\n\tto the one you have entered:\n";
					printObject( *closest );			// printing iterator value by using a printObject functor
					//		printObject() declared before switch statement
					cout << endl;
					break;
						}
				case 5: {
					printTransfers();
					cout << endl;
					break;
						}
				case 6: {
					string start, end;
					cout << "Enter start stop ID:\t";
					cin >> start;
					transform( start.begin(), start.end(), start.begin(), toupper );
					cout << "Enter end stop ID:\t";
					cin >> end;
					transform( end.begin(), end.end(), end.begin(), toupper );
					shortestpath( start, end );
					break;
						}
				case 7: {
					cout << "Enter Stop ID:\t";
					cin >> key;
					transform( key.begin(), key.end(), key.begin(), toupper );
					cout << "Please note that stops may have their north and/or south\n"
						<< "\tdirections mapped as a transfer point." << endl;
					findTransfer( key );
					break;
						}
				case 8: {
					double lat_1, lon_1, lat_2, lon_2;
					cout << "Enter Latitude and Longitude of your start stop:\n";
					cout << "Latitude: ";
					cin >> lat_1;							// type double; declared before switch statement
					cout << "Longitude: ";
					cin >> lon_1;							// type double; declared before switch statement
					closest_stop stop_1( lat_1, lon_1 );
					vector<trainStopData>::iterator closest_1 = select_item( itrStart, itrEnd, stop_1 ); // returns iterator
					cout << "Enter Latitude and Longitude of your end stop:\n";
					cout << "Latitude: ";
					cin >> lat_2;							// type double; declared before switch statement
					cout << "Longitude: ";
					cin >> lon_2;							// type double; declared before switch statement
					closest_stop stop_2( lat_2, lon_2 );
					vector<trainStopData>::iterator closest_2 = select_item( itrStart, itrEnd, stop_2 ); // returns iterator
					cout << "Best matches of location closest\n\tto the one you have entered:\n";
					printObject( *closest_1 );				// printing iterator value by using a printObject functor
					printObject( *closest_2 );				// printing iterator value by using a printObject functor
					shortestpath( closest_1 -> get_id(), closest_2 -> get_id() );

					cout << endl;
					break;
						}
				case 0: {
					cout << "////////////////////////////////////////////////////////////"
						<< "//\n//\n"
						<< "//\tTHANKS FOR USING MTA INFO CENTER, TRAVEL SAFE!\n"
						<< "//\n//\n" << endl;

					for ( int i(0); i < 5; ++i ) cout << "\n" << endl;
					break;
						}
				case -1: {
					cout << "Terminating the program" << endl;
					return;
						 }
				default: {
					cout << "That is not a valid option. Try again." << endl;
					break;
						 }

				}
				cout << endl;
			}
		}
	}

	void reboot() {
		sizes();
		clear_class();
		sizes();
		reload();
	}

private:
	vector<trainStopData> data;
	vector<list<int>> adjacent;
	map<string, int> stop_id_to_int;
	vector<string> int_to_stop_id;
	bool active;
	string train_data, transfers;

	// ================================== For HW04B Shortest Path =================================

	void shortestpath( const string& s, const string& e ) {
		shortestpaths( adjacent, s, e, stop_id_to_int, int_to_stop_id );
	}

	// ===================================== Read Data Function ===================================

	int readTrainStopData() {
		ifstream ifs( train_data );
		if ( !ifs ) {
			cerr << "File has not opened successfully." << endl;
			return -1;
		}

		string line;
		getline( ifs, line );					// skips the first line

		string id, name;
		double lat, lon;
		vector<string> tokens;
		int counter(0);

		// code in this loop is given in lecture - I altered it to look better
		while ( getline( ifs, line ) ) {
			tokens.clear();
			char str[105];						// the longest line has 103 character in the file - as a safety measure I am using 105
			strcpy( str, line.c_str() );		// assign str to hold the same value as line
			char* pch;							// will go through the char array
			pch = strtok( str, "," );			// establishing delimiter
			while ( pch ) {
				tokens.push_back( pch );
				pch = strtok( nullptr, "," );	// establishing where to begin new string
			}
			id = tokens[0];
			name = tokens[1];
			double lat =  atof( tokens[2].c_str() );
			double lon =  atof( tokens[3].c_str() );
			data.push_back( trainStopData( id, name, lat, lon ) );
			int_to_stop_id.push_back( tokens[0] );
			stop_id_to_int[tokens[0]] = counter;
			++counter;
			delete [] pch;
		}

		ifs.close();
		return 0;
	}

	bool info_collect() {
		int first = readTrainStopData();
		adjacent.resize( data.size() );
		int second = readTransfers();
		sort_adjacencies();
		if ( first == 0 && second == 0 ) return true;
		else return false;
	}

	// ================================== Functors and Functions ==================================

	class isStopOnRoute {
	public:
		isStopOnRoute( const char& route )
			: route( route ) {}

		bool operator() ( trainStopData object ) { return object.get_id().front() == route; }
	private:
		char route;
	};

	class isSubwayStop {
	public:
		isSubwayStop( const string& stopId )
			: stopId( stopId ) {}

		bool operator() ( const trainStopData& object ) { return stopId == object.get_id(); }
	private:
		string stopId;
	};

	class isSubwayStopNearX {
	public:
		isSubwayStopNearX( const double& latitude, const double& longitude, const double& d )
			: longitude( longitude ), latitude( latitude ), d( d ) {}

		bool operator() ( const trainStopData& object ) {
			return haverdist( latitude, longitude, object.get_latitude(), object.get_longitude() ) <= d;
		}
	private:
		double latitude, longitude, d;
	};

	class printTrainStopInfo {
	public:
		void operator() ( const trainStopData& object ) {
			cout << "Station Information:\n"
				<< "\t" << object.get_stop_name() << " (" << object.get_id() << ")\n"
				<< "\t(" << object.get_latitude() << ", " << object.get_longitude() << ")\n";
		}
	};

	template <class iter, class A, class B>
	int perform_if( iter itrStart, iter itrEnd, A pred, B op ) {
		int how_many( 0 );

		while ( itrStart != itrEnd ) {
			// given
			if (pred(*itrStart))
			{
				op(*itrStart);
				how_many++;
			}
			++itrStart;
		}

		return how_many;
	}

	template <class iter, class A, class B>
	int sort_stations( double lat, double lon, iter itrStart, iter itrEnd, A pred, B op ) {
		int how_many( 0 );
		vector<trainStopData> sortable;
		while ( itrStart != itrEnd ) {
			if ( pred( *itrStart ) )
			{
				sortable.push_back( *itrStart );
				++how_many;
			}
			++itrStart;
		}
		sortFunc func( lat, lon );
		sort( sortable.begin(), sortable.end(), func );
		vector<trainStopData>::iterator iter = sortable.begin();
		while ( iter != sortable.end() ) {
			op( *iter );
			++iter;
		}
		cout << "Sorted!" << endl;
		return how_many;
	}

	class sortFunc {
		double lat, lon;
	public:
		sortFunc( double lat, double lon )
			: lat( lat ), lon( lon ) {}
		bool operator()( trainStopData lhs, trainStopData rhs ) {
			if ( haverdist( lat, lon, lhs.get_latitude(), lhs.get_longitude() )
				< haverdist( lat, lon, rhs.get_latitude(), rhs.get_longitude() ) )
				return true;
			else return false;
		}
	};

	// Pre-Condition: both start and end iterators are required,
	//		and an object (predicate) to test using the overloaded operator()
	// Post-Condition: returns the last iterator that pred tests true, else returns end iterator
	template <class iter, class Pred>
	iter select_item( iter start, iter end, Pred pred ) {
		iter tmp_itr = end;
		while ( start != end ) {
			if ( pred( *start ) )
				tmp_itr = start;
			++start;
		}
		return tmp_itr;
	}

	class closest_stop {
	public:
		closest_stop( double latitude, double longitude )
			:latitude( latitude ), longitude( longitude ), shortest_dis( numeric_limits<double>::max() ) {}

		bool operator() ( const trainStopData& object ) {
			double dis = haverdist( latitude, longitude, object.get_latitude(), object.get_longitude() );
			if ( dis < shortest_dis ) {
				shortest_dis = dis;
				return true;
			}
			return false;
		}

	private:
		double latitude, longitude, shortest_dis;
	};

	class compare {
	public:
		bool operator()( const pair<string, list<string>>& tuple, const string& s ) {
			return tuple.first < s;
		}

		bool operator()( const string& s, const pair<string, list<string>>& tuple ) {
			return tuple.first < s;
		}
	};

	// ================================== Transfer Functions ======================================

	void sort_adjacencies() {
		/*for ( int i(1); i < data.size(); ++i ) {
			if ( data[i].get_id()[0] == data[i - 1].get_id()[0] ) {
				adjacent[i - 1].push_front( i );
				adjacent[i].push_front( i - 1 );
			}
		}*/

		// Code below is from HW03C solutions
		// My code above is missing a condition and has one incomplete condition
		for( int i(0); i < adjacent.size(); ++i ) {
			if( i > 0 && int_to_stop_id[i][0] == int_to_stop_id[i - 1][0])
				adjacent[i].push_back( i - 1 );
			if ( i < adjacent.size() - 1 && int_to_stop_id[i][0] == int_to_stop_id[i + 1][0])
				adjacent[i].push_back( i + 1 );
		}
	}

	void printTransfers() {
		if ( active ) {
			for ( int i(0); i < adjacent.size(); ++i ) {
				cout << "adjacent[" << int_to_stop_id[i] << "] -> ";
				for ( list<int>::iterator j = adjacent[i].begin(); j != adjacent[i].end(); ++j ) {
					cout << int_to_stop_id[*j] << " -> ";
				}
				cout << endl;
			}
		}
	}

	void findTransfer( const string& ID ) {
		cout << "Transfers at [" << ID << "] -> ";
		for ( int i(0); i < adjacent.size(); ++i ) {
			for ( list<int>::iterator j = adjacent[i].begin(); j != adjacent[i].end(); ++j ) {
				if ( int_to_stop_id[i] == ID )
					cout << int_to_stop_id[*j] << " -> ";
			}
		}
		cout << endl;
	}

	void tokenize_transfers( char s[],  vector<string>& tokens, const char delim[] ) {
		tokens.clear();
		char* pch;
		pch = strtok( s, delim );
		while ( pch ) {
			string tmp = pch;
			tokens.push_back( pch );
			pch = strtok ( NULL, delim );
		}
	}

	int readTransfers() {
		ifstream ifs( transfers );
		if ( !ifs ) {
			cerr << "File has not opened successfully." << endl;
			return -1;
		}

		string line; 
		char delim[] = ", ";
		vector<string> tokens;
		int i(0);

		getline( ifs, line );									// Skip first line

		while( getline( ifs, line ) ) {
			char* str = new char[line.size() + 1];
			strcpy( str, line.c_str() );
			tokenize_transfers( str, tokens, delim );
			if ( tokens[0] != tokens[1] ) {
				adjacent[stop_id_to_int[tokens[0]]].push_back( stop_id_to_int[tokens[1]] );
			}
			delete[] str;
		}

		ifs.close();
		return 0;
	}

	// =================================== Rebooting Functions ====================================

	void sizes() {
		cout << "\t!\t\tData size:\t" << data.size() << "\t\t\t!" << endl;
		cout << "\t!\t\tAdjacent size:\t" << adjacent.size() << "\t\t\t!" << endl;
		cout << "\t!\t\tStop ID size:\t" << int_to_stop_id.size() << "\t\t\t!" << endl;
		cout << "\t!\t\tMap ID size:\t" << stop_id_to_int.size() << "\t\t\t!" << endl;
	}

	void clear_class() {
		data.clear();
		adjacent.clear();
		int_to_stop_id.clear();
		stop_id_to_int.clear();
		active = false;
		cout << "\t!\t\tCleared\t\t\t\t\t!" << endl;
	}

	void reload() {
		bool status = info_collect();
		active = status;
		if ( active ) {
			cout << "\t!\t\tReloaded\t\t\t\t!" << endl;
			sizes();
			menu();
		}
	}
};

int main() {
	MTA mta( "MTA_train_stop_data.txt", "transfers.txt" );
	while ( true ) mta.reboot();
}