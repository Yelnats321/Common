#include "stdafx.h"
#include "OGLWrapper.h"
#include <fstream>
#include <sstream>
#include <iostream>
//#include <boost/bimap.hpp>


//extern "C"{
//#include <SOIL.h>
//}

//#include "Material.h"
//#include "Mesh.h"
//#include "ObjFile.h"


namespace gl {
using std::string;

//unordered_map<string, GLuint> textures;
//unordered_map<string, unique_ptr<ObjFile> > files;

Program::Program() {}

Program::Program(Program && rhs) {
	std::swap(data, rhs.data);
}
Program& Program::operator=(Program&& rhs) {
	std::swap(data, rhs.data);
	return *this;
}

Program::~Program() {
	glDeleteProgram(data);
}

void checkShader(const string & name, GLuint shader) {
	GLint status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status) {
		std::cout << name << " compiled" << std::endl;
	}
	else {
		std::cerr << name << " compile error" << std::endl;
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		if (length) {
			GLint infoLength;
			char* infoBuf = new char[length];
			glGetShaderInfoLog(shader, length, &infoLength, infoBuf);
			std::cerr << infoBuf;
			delete[] infoBuf;
		}
	}
}

void Program::gen(string vert, string frag, string geo) {
	std::ifstream file;

	string vertexSource, fragmentSource;
	file.open(vert);
	if (file) {
		file.seekg(0, std::ios::end);
		vertexSource.resize(static_cast<string::size_type>(file.tellg()));
		file.seekg(0, std::ios::beg);
		file.read(&vertexSource[0], vertexSource.size());
		file.close();
	}
	else throw "No vertex shader";

	file.open(frag);
	if (file) {
		file.seekg(0, std::ios::end);
		fragmentSource.resize(static_cast<string::size_type>(file.tellg()));
		file.seekg(0, std::ios::beg);
		file.read(&fragmentSource[0], fragmentSource.size());
		file.close();
	}
	else throw "No fragment shader";

	string geometrySource;
	if (!geo.empty()) {
		file.open(geo);
		if (file) {
			file.seekg(0, std::ios::end);
			geometrySource.resize(static_cast<string::size_type>(file.tellg()));
			file.seekg(0, std::ios::beg);
			file.read(&geometrySource[0], geometrySource.size());
			file.close();
		}
		else throw "No geometry shader";
	}

	// Create and compile the vertex shader
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	const char *c_str1 = vertexSource.c_str();
	glShaderSource(vertShader, 1, &c_str1, nullptr);
	glCompileShader(vertShader);

	// Create and compile the fragment shader
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char *c_str2 = fragmentSource.c_str();
	glShaderSource(fragShader, 1, &c_str2, nullptr);
	glCompileShader(fragShader);

	checkShader(vert, vertShader);
	checkShader(frag, fragShader);
	GLuint geoShader = 0;
	if (!geo.empty()) {
		geoShader = glCreateShader(GL_GEOMETRY_SHADER);
		const char *c_str3 = geometrySource.c_str();
		glShaderSource(geoShader, 1, &c_str3, nullptr);
		glCompileShader(geoShader);
		checkShader(geo, geoShader);
	}
	endl(std::cout);
	// Link the vertex and fragment shader into a shader program
	data = glCreateProgram();
	glAttachShader(data, vertShader);
	if (!geo.empty()) {
		glAttachShader(data, geoShader);
	}
	glAttachShader(data, fragShader);
	glLinkProgram(data);
	glDetachShader(data, vertShader);
	glDetachShader(data, fragShader);
	if (!geo.empty()) {
		glDetachShader(data, geoShader);
		glDeleteShader(geoShader);
	}
	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}

//void Texture::gen(string name, bool sRGB, bool mipmap){
//	if(textures.find(name) != textures.end()){
//		std::cout << " -Returned loaded texture " + name + " at location " << textures[name] << std::endl;
//		data = textures[name];
//		return;
//	}
//	gen();
//	glBindTexture(GL_TEXTURE_2D, data);
//	int width, height, channels;
//	unsigned char* image = SOIL_load_image(name.c_str(), &width, &height, &channels, SOIL_LOAD_RGB);
//	if(sRGB)
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//	else
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
//	if(mipmap)
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 8);
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
//	//TODO: this doesn't work on radeon cards maybe
//	glEnable(GL_TEXTURE_2D);
//	glGenerateMipmap(GL_TEXTURE_2D);
//
//	SOIL_free_image_data(image);
//	if(image == nullptr){
//		std::cout << " ***Error loading texture " + name + " due to " + SOIL_last_result() << std::endl;
//		throw std::exception("Texture load error");
//	}
//	std::cout << " -Loaded texture " + name + " to the location " << data << std::endl;
//	textures[name] = data;
//}

//void locationAppend(string origin, string & file){
//	std::replace(origin.begin(), origin.end(), '/', '\\');
//	if(origin.find_last_of('\\') != string::npos){
//		file = origin.substr(0, origin.find_last_of('\\') + 1) + file;
//	}
//}
//
////remember to extract curr location 
//unordered_map<string, Material> loadMaterialLibrary(string name){
//	std::cout << "-Loading the material library " + name << std::endl;
//	unordered_map<string, Material> mats;
//
//	std::ifstream file(name, std::ios::in);
//	if(file.fail()){
//		std::cout << "***Error reading materials file";
//		return mats;
//	}
//
//	Material * currMat = nullptr;
//
//	string line;
//	while(std::getline(file, line)){
//		std::istringstream ss(std::move(line));
//		string key;
//		ss >> key;
//
//		if(key.length() == 0)
//			continue;
//
//		if(key.at(0) == '#')
//			continue;
//
//		if(key == "newmtl"){
//			string matName;
//			ss >> matName;
//			std::cout << " New material: " + matName << std::endl;
//			mats.emplace(matName, Material());
//			currMat = &mats.find(matName)->second;
//		}
//
//		else if(key == "Ns" || key == "Ni" || key == "illum" || key == "Tr" || key == "d"){
//			float value = 0;
//			ss >> value;
//
//			if(key == "Ns")
//				currMat->Ns = value;
//
//
//			else if(key == "Ni")
//				currMat->Ni = value;
//
//
//			else if(key == "illum")
//				currMat->illum = (int)value;
//
//			else
//				currMat->Tr = value;
//		}
//
//		else if(key == "Tf" || key == "Ka" || key == "Kd" || key == "Ks"){
//			float a, b, c;
//			a = b = c = 0;
//			ss >> a >> b >> c;
//
//			if(key == "Tf"){
//				currMat->Tf[0] = a;
//				currMat->Tf[1] = b;
//				currMat->Tf[2] = c;
//			}
//
//			else if(key == "Ka"){
//				currMat->Ka[0] = a;
//				currMat->Ka[1] = b;
//				currMat->Ka[2] = c;
//			}
//
//			else if(key == "Kd"){
//				currMat->Kd[0] = a;
//				currMat->Kd[1] = b;
//				currMat->Kd[2] = c;
//			}
//
//			else{
//				currMat->Ks[0] = a;
//				currMat->Ks[1] = b;
//				currMat->Ks[2] = c;
//			}
//		}
//
//		else if(key == "map_Ka" || key == "map_Kd" || key == "map_d" || key == "bump" || key == "map_bump"){
//			string mapName;
//			ss >> mapName;
//			locationAppend(name, mapName);
//			bool sRGB = !(key == "bump" || key == "map_bump");
//			gl::Texture texture; 
//			texture.gen(mapName, sRGB);
//
//			if(key == "map_Ka")
//				currMat->map_Ka = texture;
//
//			else if(key == "map_Kd")
//				currMat->map_Kd = texture;
//
//			else if(key == "map_d")
//				currMat->map_d = texture;
//
//			//if it isn't any of those maps, it must be bump map
//			else
//				currMat->map_bump = texture;
//			//HACK: CHANGE THIS THIS IS NOT A GOOD WAY TO HANDLE THIS
//			texture.data = 0;
//		}
//
//		else{
//			std::cout << ss.str() << std::endl;
//		}
//	}
//
//	return std::move(mats);
//}
//
//struct VertexData{
//	GLuint pos, tex, norm;
//	VertexData(GLuint a, GLuint b, GLuint c):pos(a), tex(b), norm(c){}
//};
//
//inline bool operator==(const VertexData & lhs, const VertexData &rhs){
//	return(lhs.pos == rhs.pos && lhs.tex == rhs.tex && lhs.norm == rhs.norm);
//}
//inline bool operator<(const VertexData & lhs, const VertexData & rhs){
//	if(lhs.pos != rhs.pos)
//		return lhs.pos < rhs.pos;
//	if(lhs.norm != rhs.norm)
//		return lhs.norm < rhs.norm;
//	return lhs.tex < rhs.tex;
//}
//
//typedef boost::bimap<VertexData, GLuint> bm_type;
//typedef bm_type::value_type bm_value;
//
//GLuint addCache(bm_type & vertexCache, GLuint a, GLuint b, GLuint c){
//	const auto & it = vertexCache.left.find(VertexData(a, b, c));
//	if(it != vertexCache.left.end()){
//		return it->second;
//	}
//	vertexCache.insert(bm_value(VertexData(a, b, c), vertexCache.size()));
//	return vertexCache.size() - 1;
//}
//
//bool parseObj(string name, vector<glm::vec3> & positions, vector<glm::vec2> & textures, vector<glm::vec3> & normals,
//			  bm_type & vertexCache, vector<GLuint> & elements,
//			  vector<std::pair<string, int> > & matCalls, unordered_map<string, Material> & matLib,
//			  vector<std::pair<string, int> > & groups){
//	std::cout << "Loading file " + name << std::endl;
//	std::ifstream file(name, std::ios::in);
//	if(file.fail()){
//		std::cout << "Error reading file " + name << std::endl;;
//		return false;
//	}
//
//	string line;
//	while(std::getline(file, line)){
//		std::istringstream ss(std::move(line));
//		string key;
//		ss >> key;
//
//		if(key.length() == 0)
//			continue;
//
//		if(key.at(0) == '#')
//			continue;
//
//		if(key == "v" || key == "vt" || key == "vn"){
//			float x, y, z;
//			ss >> x >> y >> z;
//			if(key == "v")
//				positions.emplace_back(x, y, z);
//			else if(key == "vt")
//				textures.emplace_back(x, y);
//			else
//				normals.emplace_back(x, y, z);
//		}
//
//		else if(key == "g"){
//			string groupName;
//			ss >> groupName;
//			if(groups.size() > 0 && groups.back().second == elements.size())
//				groups.back().first = std::move(groupName);
//			else
//				groups.emplace_back(std::move(groupName), elements.size());
//		}
//
//		else if(key == "mtllib"){
//			string matName;
//			ss >> matName;
//			locationAppend(name, matName);
//			std::cout << "Started mat lib load " << glfwGetTime() << "\n";
//			matLib = std::move(loadMaterialLibrary(matName));
//			std::cout << "Ended mat lib load " << glfwGetTime() << "\n";
//		}
//
//		else if(key == "usemtl"){
//			string mtlName;
//			ss >> mtlName;
//			if(matCalls.size() > 0 && matCalls.back().second == elements.size())
//				matCalls.back().first = std::move(mtlName);
//			else
//				matCalls.emplace_back(std::move(mtlName), elements.size());
//		}
//		//faces
//		else if(key == "f"){
//			for(int i = 0; i < 3; ++i){
//				GLuint a, b, c;
//				a = b = c = 0;
//
//				ss >> a;
//				if(ss.get() == '/'){
//					if(ss.peek() != '/'){
//						ss >> b;
//					}
//					if(ss.get() == '/'){
//						ss >> c;
//					}
//				}
//				elements.emplace_back(addCache(vertexCache, a, b, c));
//
//				if(i == 2){
//					ss >> std::ws;
//					if(!ss.eof()){
//						elements.emplace_back(addCache(vertexCache, a, b, c));
//						a = b = c = 0;
//
//						ss >> a;
//						if(ss.get() == '/'){
//							if(ss.peek() != '/'){
//								ss >> b;
//							}
//							if(ss.get() == '/'){
//								ss >> c;
//							}
//						}
//
//						elements.emplace_back(addCache(vertexCache, a, b, c));
//
//						elements.emplace_back(elements[elements.size() - 5]);
//					}
//				}
//			}
//		}
//	}
//	if(groups.size() == 0)
//		groups.emplace_back("default", 0);
//	return true;
//}
//
//void calculateTangents(vector<glm::vec3> & positions, vector<glm::vec2> & textures, vector<glm::vec3> & normals,
//					   bm_type & vertexCache, vector<GLuint> & elements,
//					   vector<glm::vec3>& tangents, vector<glm::vec3> & bitangents){
//
//	for(int i = 0; i < elements.size(); i += 3){
//		const VertexData & p1 = vertexCache.right.at(elements[i]);
//		const VertexData & p2 = vertexCache.right.at(elements[i + 1]);
//		const VertexData & p3 = vertexCache.right.at(elements[i + 2]);
//		glm::vec3 & v0 = positions[p1.pos - 1];
//		glm::vec3 & v1 = positions[p2.pos - 1];
//		glm::vec3 & v2 = positions[p3.pos - 1];
//
//		glm::vec2 & uv0 = textures[p1.tex - 1];
//		glm::vec2 & uv1 = textures[p2.tex - 1];
//		glm::vec2 & uv2 = textures[p3.tex - 1];
//
//		glm::vec3 deltaPos1 = v1 - v0;
//		glm::vec3 deltaPos2 = v2 - v0;
//
//		glm::vec2 deltaUV1 = uv1 - uv0;
//		glm::vec2 deltaUV2 = uv2 - uv0;
//
//		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
//		glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
//		glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;
//		for(int j = 0; j < 3; j++){
//			const VertexData & p = vertexCache.right.at(elements[i + j]);
//			glm::vec3 & n = normals[p.norm - 1];
//			glm::vec3 t = tangent;
//			glm::vec3 b = bitangent;
//
//			// Gram-Schmidt orthogonalize
//			t = glm::normalize(t - n * glm::dot(n, t));
//
//			// Calculate handedness
//			if(glm::dot(glm::cross(n, t), b) < 0.0f){
//				t = t * -1.0f;
//			}
//
//			tangents[elements[i + j]] += t;
//			bitangents[elements[i + j]] += b;
//		}
//	}
//	/*for(int i = 0; i < vertexCache.size(); i++){
//	const VertexData & p = vertexCache.right.at(i);
//	glm::vec3 & n = normals[p.norm-1];
//	glm::vec3 & t = tangents[i];
//	glm::vec3 & b = bitangents[i];
//
//	// Gram-Schmidt orthogonalize
//	t = glm::normalize(t - n * glm::dot(n, t));
//
//	// Calculate handedness
//	if (glm::dot(glm::cross(n, t), b) < 0.0f){
//	t = t * -1.0f;
//	}
//	}*/
//}

//const ObjFile * loadFile(string name){
//	if(files.find(name) != files.end()){
//		std::cout << "Retrieved file " + name << std::endl;
//		return files.find(name)->second.get();
//	}
//
//	vector<glm::vec3> positions;
//	vector<glm::vec2> textures;
//	vector<glm::vec3> normals;
//	vector<GLuint> elements;
//
//	boost::bimap<VertexData, GLuint> vertexCache;
//	vector<std::pair<string, int> > groups;
//	vector<std::pair<string, int> > masterMatCalls;
//	unordered_map<string, Material>matLib;
//	if(!parseObj(name, positions, textures, normals, vertexCache, elements, masterMatCalls, matLib, groups))
//		return nullptr;
//
//	std::cout << glfwGetTime() << std::endl;
//	bool useTextures = false, useNormals = false;
//
//	if(vertexCache.begin()->left.tex != 0)
//		useTextures = true;
//	if(vertexCache.begin()->left.norm != 0)
//		useNormals = true;
//	//REMEMBER SPONGEBOB
//	//RAVIOLI RAVIOLI
//	//DON'T FORGET THE SUBTRACTIOLI
//	//OBJ COUNTS FROM 1 NOT 0
//
//
//	vector<glm::vec3> tangents(vertexCache.size());
//	vector<glm::vec3> bitangents(vertexCache.size());
//	if(useNormals && useTextures){
//		calculateTangents(positions, textures, normals, vertexCache, elements, tangents, bitangents);
//	}
//
//
//	GLuint blockSize = 3 + (useTextures ? 2 : 0) + (useNormals ? 3 : 0);
//	if(blockSize == 8)
//		blockSize = 14;
//	vector<float> data(vertexCache.size() *blockSize, 0);
//	for(GLuint k = 0; k < vertexCache.size(); ++k){
//		GLuint dataPos = k*blockSize;
//
//		const VertexData & point = vertexCache.right.at(k);
//
//		data[dataPos] = positions[point.pos - 1].x;
//		data[dataPos + 1] = positions[point.pos - 1].y;
//		data[dataPos + 2] = positions[point.pos - 1].z;
//		if(useTextures){
//			data[dataPos + 3] = textures[point.tex - 1].x;
//			data[dataPos + 4] = textures[point.tex - 1].y;
//			dataPos += 2;
//		}
//
//		if(useNormals){
//			//std::copy(normals.begin() + (vertexData[elemPos+2]-1)*3, normals.begin()+(vertexData[elemPos+2]-1)*3+3, data.begin()+dataPos+5);
//			//std::copy(&normals[(vertexData[elemPos+2]-1)*3], &normals[(vertexData[elemPos+2]-1)*3+3], &data[dataPos+5]);
//			data[dataPos + 3] = normals[point.norm - 1].x;
//			data[dataPos + 4] = normals[point.norm - 1].y;
//			data[dataPos + 5] = normals[point.norm - 1].z;
//		}
//
//		if(useTextures && useNormals){
//			data[dataPos + 6] = tangents[k].x;
//			data[dataPos + 7] = tangents[k].y;
//			data[dataPos + 8] = tangents[k].z;
//			data[dataPos + 9] = bitangents[k].x;
//			data[dataPos + 10] = bitangents[k].y;
//			data[dataPos + 11] = bitangents[k].z;
//		}
//
//	}
//
//
//
//	GLuint vbo = 0;
//	glGenBuffers(1, &vbo);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), &data[0], GL_STATIC_DRAW);
//	//ObjFile * objFile = new ObjFile(vbo, useTextures, std::move(matLib));
//	files.emplace(name, std::make_unique<ObjFile>(vbo, useTextures, std::move(matLib)));
//	ObjFile * objFile = files[name].get();
//
//	for(int j = 0; j<groups.size(); ++j){
//		const int currPos = groups[j].second;
//		int nbGroupVertices;
//		if(j + 1 == groups.size())
//			nbGroupVertices = elements.size() - currPos;
//		else
//			nbGroupVertices = groups[j + 1].second - currPos;
//
//		std::vector<std::pair<string, int>> matCalls;
//		for(int mat = 0; mat < masterMatCalls.size(); ++mat){
//			//if this material call is the less than or equal to the group start AND the next one is too big for the group start OR the next one is the end
//			if(masterMatCalls[mat].second <= currPos && (mat + 1 == masterMatCalls.size() || masterMatCalls[mat + 1].second > currPos))
//				matCalls.emplace_back(masterMatCalls[mat].first, 0);
//			else if(masterMatCalls[mat].second > currPos && masterMatCalls[mat].second < currPos + nbGroupVertices)
//				matCalls.emplace_back(masterMatCalls[mat].first, masterMatCalls[mat].second - currPos);
//		}
//		gl::VAO vao;
//		gl::Buffer ebo;
//		vao.gen();
//		glBindVertexArray(vao);
//
//		ebo.gen();
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, nbGroupVertices * sizeof(GLuint), &elements[currPos], GL_STATIC_DRAW);
//
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, blockSize*sizeof(float), 0);
//		glEnableVertexAttribArray(0);
//		if(useTextures){
//			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, blockSize*sizeof(float), (void*)(sizeof(float) * 3));
//			glEnableVertexAttribArray(1);
//		}
//		if(useNormals){
//			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, blockSize*sizeof(float), (void*)(sizeof(float)*(useTextures ? 5 : 3)));
//			glEnableVertexAttribArray(2);
//		}
//		if(useNormals && useTextures){
//			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, blockSize*sizeof(float), (void*)(sizeof(float) * 8));
//			glEnableVertexAttribArray(3);
//			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, blockSize*sizeof(float), (void*)(sizeof(float) * 11));
//			glEnableVertexAttribArray(4);
//		}
//
//		auto x = std::min_element(std::begin(positions), std::end(positions), [](const glm::vec3 & f, const glm::vec3 & s){
//			return f.x < s.x;
//		});
//		auto y = std::min_element(std::begin(positions), std::end(positions), [](const glm::vec3 & f, const glm::vec3 & s){
//			return f.y < s.y;
//		});
//		auto z = std::min_element(std::begin(positions), std::end(positions), [](const glm::vec3 & f, const glm::vec3 & s){
//			return f.z < s.z;
//		});
//		auto minBounds = glm::vec3{x->x, y->y, z->z};
//		x = std::max_element(std::begin(positions), std::end(positions), [](const glm::vec3 & f, const glm::vec3 & s){
//			return f.x < s.x;
//		});
//		y = std::max_element(std::begin(positions), std::end(positions), [](const glm::vec3 & f, const glm::vec3 & s){
//			return f.y < s.y;
//		});
//		z = std::max_element(std::begin(positions), std::end(positions), [](const glm::vec3 & f, const glm::vec3 & s){
//			return f.z < s.z;
//		});
//
//		auto maxBounds = glm::vec3{x->x, y->y, z->z};
//		objFile->addMesh(groups[j].first, 
//						 std::make_unique<Mesh>(objFile, std::move(vao), std::move(ebo), 
//												nbGroupVertices, std::move(matCalls), 
//												minBounds, maxBounds));
//	}
//	std::cout << glfwGetTime() << std::endl;
//	std::cout << " Stats " << std::endl
//		<< data.size() / blockSize << " " << elements.size() << std::endl;
//	return objFile;
//}
}