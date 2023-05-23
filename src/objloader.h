#ifndef OBJLOADER_H
#define OBJLOADER_H

// Modified from 
// https://github.com/opengl-tutorials/ogl/blob/master/common/objloader.cpp

#include "gl_env.h"
#include "scene.h"
#include "triangle.h"
#include <vector>


bool loadOBJ(const char * path, Scene *s, int sceneid, glm::fvec3 d, glm::fvec3 u){
	printf("Loading OBJ file %s...\n", path);

	std::vector<glm::vec3> temp_vertices; 
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;

	bool parsing = true;

	FILE * file = fopen(path, "r");
    
	if( file == NULL ){
		printf("Unable to read file\n");
		return false;
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vt" ) == 0 ){
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y );
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			
			if(parsing){
				parsing = false;

				if(temp_uvs.empty())temp_uvs.resize(1);

				glm::fvec3 _u(-INFINITY), _d(INFINITY);
				for(int i=0; i<temp_vertices.size(); ++i){
					for(int j=0; j<3; ++j){
						_u[j] = std::max(_u[j], temp_vertices[i][j]);
						_d[j] = std::min(_d[j], temp_vertices[i][j]);
					}
				}
				float scale = INFINITY;
				for(int j=0; j<3; ++j)scale = std::min(scale, (u[j] - d[j]) / (_u[j] - _d[j]));
				glm::fvec3 b = ((u - d) - (_u - _d) * scale) * 0.5f; 
				for(int i=0; i<temp_vertices.size(); ++i){
					temp_vertices[i] = (temp_vertices[i] - _d) * scale + d + b;
					for(int j=0; j<3; ++j)
						assert(d[j] <= temp_vertices[i][j] && temp_vertices[i][j] <= u[j]);
				}
			}

			std::string vertex1, vertex2, vertex3;
            static const int MAX_VERTEX_PER_POLYGON = 10;
			unsigned int vertex[MAX_VERTEX_PER_POLYGON + 1], uv[MAX_VERTEX_PER_POLYGON + 1]={0}, normal[MAX_VERTEX_PER_POLYGON + 1];
			for(int i=0; i<MAX_VERTEX_PER_POLYGON; ++i)vertex[i] = uv[i] = normal[i] = 1;
            int n = 0;
			while(1){
                // int num = fscanf(file, "%d/%d/%d", &vertex[n], &uv[n], &normal[n]);
                int num = fscanf(file, "%d", &vertex[n]);
                if (num == 0){
                    printf("Parse failed 1\n");
                    fclose(file);
                    return false;
                }
                ++n;
                if(n > MAX_VERTEX_PER_POLYGON){
                    printf("Parse failed 2\n");
                    fclose(file);
                    return false;
                }
				char endChar; fscanf(file, "%c", &endChar);
				if(endChar == ' ');
				else if(endChar == '\n'){
					break;
				}else{
                    printf("Parse failed 3\n");
                    fclose(file);
                    return false;
				}
            }
			if(n < 3){
				printf("Parse failed\n");
				fclose(file);
				return false;
			}
            for(int i=1; i+1<n; ++i){
				s -> addShape(
					new Triangle(temp_vertices[vertex[0]-1], temp_vertices[vertex[i]-1], temp_vertices[vertex[i+1]-1],
						temp_uvs[uv[0]-1], temp_uvs[uv[i]-1], temp_uvs[uv[i+1]-1],
						glm::fvec3(0.0),//(1.0f/3) * (temp_normals[normal[0]] + temp_normals[normal[i]] + temp_normals[normal[i+1]]),
						sceneid));
            }
		}else{
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	fclose(file);
	return true;
}

#endif
