#include "SimulationPlane.hpp"
#include <limits>
#include <iostream>
#include <cmath>

//projecion to space
sf::Vector3f SimulationPlane::SpaceTransformation(sf::Vector3f p) const{

         //rotation matrices
         matrix Mx;
         Mx.data[0][0] = 1;
         Mx.data[1][1] = cos(rotation.x);
         Mx.data[2][2] = cos(rotation.x);
         Mx.data[3][3] = 1;
         Mx.data[2][1] = -sin(rotation.x);
         Mx.data[1][2] = sin(rotation.x);

         matrix My;
         My.data[0][0] = cos(rotation.y);
         My.data[1][1] = 1;
         My.data[2][2] = cos(rotation.y);
         My.data[3][3] = 1;
         My.data[0][2] = sin(rotation.y);
         My.data[2][0] = -sin(rotation.y);

         matrix Mz;
         Mz.data[0][0] = cos(rotation.z);
         Mz.data[1][1] = cos(rotation.z);
         Mz.data[1][0] = -sin(rotation.z);
         Mz.data[0][1] = sin(rotation.z);
         Mz.data[2][2] = 1;
         Mz.data[3][3] = 1;

         vec out;
         vec inp;
         inp.set(p.x, p.y, p.z, 1);
         out = Mz*Mx*inp;

         return sf::Vector3f(out.data[0], out.data[1], out.data[2]);
}

//projection on the screen
sf::Vector2f SimulationPlane::ScreenProjection(sf::Vector3f p) const {
        double n = 2;
        double f = -1;
        double l = -1;
        double r = 1;
        double b = -1;
        double t = 1;

        matrix M;
        M.data[0][0] = 2*n/(r-l);
        M.data[1][1] = 2*n/(t-b);
        M.data[2][2] = -(f+n)/(f-n);
        M.data[2][0] = (r+l)/(r-l);
        M.data[2][1] = (t+b)/(t-b);
        M.data[2][3] = -1;
        M.data[3][2] = -2*f*n/(f-n);

 
        vec out;
        vec inp;
	inp.set(p.x, p.y, p.z, 1);
	out = M*inp;
        out.data[0] = 300+(out.data[0]-l)*SizeX/(r-l);  //plus some offset bc its fucked up
        out.data[1] = 200+(out.data[1]-b)*SizeY/(t-b);
 
        return sf::Vector2f(out.data[0] ,out.data[1]);
}

void SimulationPlane::SortFaces(){
	faces.clear();

	size_t n = std::sqrt(m_vertices.size());
	for (int i = 0; i < n-1; i++) {
		for(int j = 0; j < n-1; j++){
			/*
			float ratio = (m_vertices[n*i + j].z - minZ) / (maxZ - minZ); //not quite right
			sf::Uint8 red = static_cast<sf::Uint8>(255 * ratio);
			sf::Uint8 blue = static_cast<sf::Uint8>(255 * (1 - ratio));
			*/


			sf::Vector3f p1; sf::Vector3f p2; sf::Vector3f p3;
			p1 = SpaceTransformation(m_vertices[n*i+j]);
			p2 = SpaceTransformation(m_vertices[n*i+j+1]);
			p3 = SpaceTransformation(m_vertices[n*(i+1)+j+1]);
			faces.push_back(Face(p1, p2, p3));


			p1 = SpaceTransformation(m_vertices[n*i+j]);
			p2 = SpaceTransformation(m_vertices[n*(i+1)+j]);
			p3 = SpaceTransformation(m_vertices[n*(i+1)+j+1]);
			faces.push_back(Face(p1, p2, p3));

		}	
	}

	//we do selection sort 
	for( int i = 0; i < 2*n-2; i++){
		double furthest_dist = faces[i].dist;
		double big_i = i; //index of element to swap
		for(int j = i; j < 2*n-2; j++){
			if( faces[j].dist > furthest_dist){
				furthest_dist = faces[j].dist;
				big_i = j;
			}
		}

		//swap
		Face tmp = faces[i];
		faces[i] = faces[big_i];
		faces[big_i] = tmp;
	}
}


void SimulationPlane::draw(sf::RenderTarget & target, sf::RenderStates states) const {
	int n = m_vertices.size();
	sf::ConvexShape tring; 
			       
	for(int i = 0; i < 2*n-2; i++){
		//this factor arbitrary kinda, it should be clamped also 
		tring.setFillColor(sf::Color(50*(9-faces[i].dist) + 12000*faces[i].normal, 0, 0)); //display some combination of depth and normals

		tring.setPointCount(3);
		tring.setPoint(0, ScreenProjection(faces[i].p1));
		tring.setPoint(1, ScreenProjection(faces[i].p2));
		tring.setPoint(2, ScreenProjection(faces[i].p3));

		target.draw(tring, states);
	}
}

void SimulationPlane::Rotate(float x, float y, float z){
	rotation.x += x; 
	rotation.y += y;
	rotation.z += z;
	while(rotation.x > 2*M_PI) rotation.x -= 2*M_PI; 
	while(rotation.y > 2*M_PI) rotation.y -= 2*M_PI; 
	while(rotation.z > 2*M_PI) rotation.z -= 2*M_PI; 
	while(rotation.x < 2*M_PI) rotation.x += 2*M_PI; 
	while(rotation.y < 2*M_PI) rotation.y += 2*M_PI; 
	while(rotation.z < 2*M_PI) rotation.z += 2*M_PI; 
}

void SimulationPlane::initialize(size_t size) {
	srand(time(NULL));
	m_vertices.clear();
	double step = 2/(double)size;
	for (size_t x = 0; x < size; x++) {
		for (size_t y = 0; y < size; y++) {
			m_vertices.push_back(sf::Vector3f(-1 +  x * step, -1 + y * step, 0.0f)); //it shouldnt matter for z? was x+y
		}
	}

	m_waveSources.clear();
	//m_waveSources.push_back(WaveSource(sf::Vector2f(50.0f, 50.0f), 1.0f, 1.0f)); 
	m_waveSources.push_back(WaveSource(sf::Vector2f(0.2f, 0.3f), 0.04f, 1.0f)); //point
	m_waveSources.push_back(WaveSource(sf::Vector2f(0.0f, 0.0f), 0.1f, 0.5f)); 
}

void SimulationPlane::simulate(float dt) {
	//todo: modify wave equation
	for (auto& vertex : m_vertices) {
		float newZ = 0.0f;
		for (const auto& waveSource : m_waveSources) {
			float distance = std::hypot(vertex.x - waveSource.origin.x, vertex.y - waveSource.origin.y);
			newZ += waveSource.amplitude * std::sin(waveSource.frequency * dt - distance*30); //paramteter 30 here
		}
		vertex.z = newZ;
	}
}
