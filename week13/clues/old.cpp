#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <vector>
#include <queue>
#include <boost/pending/disjoint_sets.hpp>

#define trace(x) std::cerr << #x << " = " << x << std::endl


struct Info{
  int id;
  int range;
};

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_with_info_2<Info,K> Vb;
typedef CGAL::Triangulation_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb,Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds>  Triangulation;
typedef K::Point_2 P;
typedef Triangulation::Vertex_handle Vertex_handle;

void testcase(){
  long n,m,r;
  std::cin >> n >> m >> r;

  std::vector<std::pair<P,Info>> stations;
  for(int i=0;i<n;++i){
    long x,y;
    std::cin >> x >> y;
    stations.push_back({P(x,y),{i,-1}});
  }

  Triangulation t;
  t.insert(stations.begin(),stations.end());

  
  bool valid=true;
  for(auto start = t.finite_vertices_begin();start != t.finite_vertices_end();++start){
    if(start->info().range!=-1) continue;
    start->info().range=0;
    std::queue<Vertex_handle> Q;
    Q.push(start);
    while(valid && !Q.empty()){
      Vertex_handle vh = Q.front(); Q.pop();
      if(t.is_infinite(vh)) continue;
      if(vh->info().range==-1){
        Triangulation::Vertex_circulator c = t.incident_vertices(vh);
        bool found0=false;
        bool found1=false;
        do{
          if(!t.is_infinite(c)){
            if(CGAL::squared_distance(c->point(),vh->point())<=r*r){
              if(c->info().range==0) found0=true;
              if(c->info().range==1) found1=true;
            }
          } 
        } while(++c != t.incident_vertices(vh));
        if(found0 && found1) {
          valid = false;
          break;
        }
        assert(found0 || found1);
        int to_assign = (found0 ? 1: 0);
        vh->info().range = to_assign;
      }
      int current = vh->info().range;
      int other = (current+1)%2;
      Triangulation::Vertex_circulator c = t.incident_vertices(vh);
      do{
        if(!t.is_infinite(c)){
          if(CGAL::squared_distance(c->point(),vh->point())<=r*r){
            if(c->info().range==-1){
              Q.push(c);
            }
            else if(c->info().range != other){
              valid = false;
              break;
            }
          }
        } 
      } while(++c != t.incident_vertices(vh));
    }
  }


  boost::disjoint_sets_with_storage<> UF(n);
  for(auto it = t.finite_edges_begin();it!=t.finite_edges_end();++it){
    Vertex_handle v1 = it->first->vertex((it->second+1)%3);
    Vertex_handle v2 = it->first->vertex((it->second+2)%3);
    if(CGAL::squared_distance(v1->point(),v2->point())<=r*r){
      UF.union_set(v1->info().id,v2->info().id); 
    }
  }

  for(int i=0;i<m;++i){
    long x0,y0,x1,y1;
    std::cin >> x0 >> y0 >> x1 >> y1;
    P a(x0,y0);
    P b(x1,y1);
    Vertex_handle v0 = t.nearest_vertex(a);
    Vertex_handle v1 = t.nearest_vertex(b);
    if(valid && CGAL::squared_distance(a,b)<=r*r){
      std::cout << 'y';  
    } else if(!valid){
      std::cout << 'n';  
    } else {
      std::cout <<'a';
    }
  }
  std::cout << std::endl;
}

int main(){
  std::ios_base::sync_with_stdio(false);
  int T;
  std::cin >> T;
  while(T--) testcase();
}
