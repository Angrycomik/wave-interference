#ifndef SimulationPlane_hpp
#define SimulationPlane_hpp

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

//change doubles to floats?

class vec {
	public:
	double data[4];

	vec(){
		for(int i = 0; i < 4; i++) data[i] = 0;
	}

	void set(double a, double b, double c, double d){ 
		data[0] = a; data[1] = b; data[2] = c; data[3] = d;
	} 

	/*	
	double operator[](const int& i){ 
		return data[i];
	}
	*/
	
};

class matrix {
	public:

	double data[4][4];


	matrix(){
		for(int i = 0; i < 4; i++){
			for(int j = 0; j < 4; j++){
				data[i][j] = 0;
			}
		}
	}

	//unused func
	void set(int i, int j, double val){ //column, row
		data[i][j] = val;
	}

	//multiply by matrix
	matrix operator*(const matrix& m){
		for(int i = 0; i < 4; i++){
			for(int j = 0; j < 4; j++){
				double val = 0;
				for(int k = 0; k < 4; k++){
					val += data[k][j]*m.data[i][k];
				}
				data[i][j] = val;
			}
		}
		return *this;
	}

	vec operator*(const vec& v){
		vec new_vec;
		new_vec.set(0, 0, 0, 0);
		for(int j = 0; j < 4; j++){
			for(int k = 0; k < 4; k++){
				new_vec.data[j] += data[k][j]*v.data[k];
			}
		}
		return new_vec;
	}	

	//try to do antther operator []
};

struct Face{
	sf::Vector3f p1; //make return by [] operator?
	sf::Vector3f p2;
	sf::Vector3f p3;
	double dist;
	double normal;

	Face(){
		dist = 0;
	}

	Face(sf::Vector3f a, sf::Vector3f b, sf::Vector3f c){
		p1 = a; p2 = b; p3 = c;
		dist = std::sqrt((p1.x*p1.x + p1.y*p1.y + p2.x*p2.x + p2.y*p2.y + p3.x*p3.x + p3.y*p3.y + (p1.z+2)*(p1.z+2)+ (p2.z+2)*(p2.z+2) + (p3.z+2)*(p3.z+2))/(double)3); //this +2 is bc camera is in front some distance
																						//but it could be any num

		//first cross product then dot product
		sf::Vector3f v1 = sf::Vector3f(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
		sf::Vector3f v2 = sf::Vector3f(p1.x - p3.x, p1.y - p3.y, p1.z - p3.z);
		normal = (double)fabs(v1.x*v2.y - v1.y*v2.x);
	}
};

struct WaveSource {
	sf::Vector2f origin;
	float amplitude;
	float frequency;

	WaveSource() : origin(0.0f, 0.0f), amplitude(0.0f), frequency(0.0f) {}
	WaveSource(sf::Vector2f origin, float amplitude, float frequency) : origin(origin), amplitude(amplitude), frequency(frequency) {}
};


class SimulationPlane : public sf::Drawable {
private:
	std::vector<sf::Vector3f> m_vertices; 
	std::vector<Face> faces;
	std::vector<WaveSource> m_waveSources;
	sf::Vector3f rotation = sf::Vector3f(0, 0, 0);
	double SizeX = 500; double SizeY = 500;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
public:
	sf::Vector3f SpaceTransformation(sf::Vector3f p) const ;
	sf::Vector2f ScreenProjection(sf::Vector3f p) const ;

	void Rotate(float x, float y, float z);
	void SortFaces();

	void initialize(size_t size);
	void simulate(float dt);
};

#endif
