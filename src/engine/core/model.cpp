#include <engine/core/model.hpp>


void Mesh::draw() const
{


	for (const auto& [type, texture] : textures) {
		texture.bind(0);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	//if (texture)
	//	texture->bind(0);

	//glBindVertexArray(VAO);
	//glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

std::string Model::s_basePath = "assets/models/";

void Model::SetBasePath(const std::string& basePath)
{
	s_basePath = basePath;
}

const std::string& Model::GetBasePath()
{
	return s_basePath;
}

void Model::draw() const
{
	for (const auto& mesh : meshes)
		mesh.draw();
	
}

Model::Model(const std::string& modelPath)
	: path(modelPath)
{
	loadModel(modelPath);
}

void Model::loadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(s_basePath + path,
		aiProcess_Triangulate
		| aiProcess_GenSmoothNormals
		| aiProcess_CalcTangentSpace
		| aiProcess_FlipUVs
		| aiProcess_CalcTangentSpace);

	if (!scene || !scene->HasMeshes()) {
		throw std::runtime_error("GGRRRRR SCENE SUCKS AT: " + path);
	}

	meshes.resize(scene->mNumMeshes);
	std::string oldPath = Texture::GetBasePath();
	Texture::SetBasePath("");

	for (int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* aMesh = scene->mMeshes[i];
		aiMaterial* aMaterial = scene->mMaterials[aMesh->mMaterialIndex];
		meshes[i] = processMesh(aMesh, aMaterial);
	}

	Texture::SetBasePath(oldPath);
}

Mesh Model::processMesh(aiMesh* aMesh, aiMaterial* aMaterial)
{
	std::vector<MVertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	vertices.reserve(aMesh->mNumVertices);

	for (int i = 0; i < aMesh->mNumVertices; i++) {
		MVertex vertex;
		vertex.position = {
			aMesh->mVertices[i].x,
			aMesh->mVertices[i].y,
			aMesh->mVertices[i].z
		};
		if (aMesh->HasNormals()) {
			vertex.normal = {
				aMesh->mNormals[i].x,
				aMesh->mNormals[i].y,
				aMesh->mNormals[i].z,
			};
		}
		else {
			vertex.normal = glm::vec3(0.0f);
		}

		if (aMesh->HasTextureCoords(0)) {
			vertex.uv = {
				aMesh->mTextureCoords[0][i].x,
				aMesh->mTextureCoords[0][i].y
			};

			vertex.tangent = {
				aMesh->mTangents[i].x,
				aMesh->mTangents[i].y,
				aMesh->mTangents[i].z
			};

			vertex.bitTangent = {
				aMesh->mBitangents[i].x,
				aMesh->mBitangents[i].y,
				aMesh->mBitangents[i].z
			};
		}
		else {
			vertex.uv = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	for (unsigned i = 0; i < aMesh->mNumFaces; i++) {
		const aiFace& face = aMesh->mFaces[i];
		for (unsigned j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	GLuint VBO, VAO, EBO;
	glCreateVertexArrays(1, &VAO);
	glCreateBuffers(1, &VBO);
	glCreateBuffers(1, &EBO);

	glNamedBufferData(VBO, vertices.size() * sizeof(MVertex), vertices.data(), GL_STATIC_DRAW);
	glNamedBufferData(EBO, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(MVertex));
	glVertexArrayElementBuffer(VAO, EBO);

	glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, offsetof(MVertex, position));
	glVertexArrayAttribBinding(VAO, 0, 0);
	glEnableVertexArrayAttrib(VAO, 0);

	glVertexArrayAttribFormat(VAO, 1, 3, GL_FLOAT, GL_FALSE, offsetof(MVertex, normal));
	glVertexArrayAttribBinding(VAO, 1, 0);
	glEnableVertexArrayAttrib(VAO, 1);

	glVertexArrayAttribFormat(VAO, 2, 2, GL_FLOAT, GL_FALSE, offsetof(MVertex, uv));
	glVertexArrayAttribBinding(VAO, 2, 0);
	glEnableVertexArrayAttrib(VAO, 2);

	std::vector<Texture> diffuseMaps = loadTextures(aMaterial, aiTextureType_DIFFUSE);
	copy(diffuseMaps.begin(), diffuseMaps.end(), back_inserter(textures));

	std::vector<Texture> specularMaps = loadTextures(aMaterial, aiTextureType_SPECULAR);
	copy(specularMaps.begin(), specularMaps.end(), back_inserter(textures));

	std::vector<Texture> normalMaps = loadTextures(aMaterial, aiTextureType_HEIGHT);
	copy(normalMaps.begin(), normalMaps.end(), back_inserter(textures));

	std::vector<Texture> heightMaps = loadTextures(aMaterial, aiTextureType_AMBIENT);
	copy(heightMaps.begin(), heightMaps.end(), back_inserter(textures));

	Mesh mesh;
	mesh.VBO = VBO;
	mesh.VAO = VAO;
	mesh.EBO = EBO;
	mesh.indexCount = indices.size();
	mesh.texture = loadTexture(aMaterial);
	
	return mesh;
}

std::vector<Texture> Model::loadTextures(aiMaterial* material, aiTextureType type) const
{

	if (material->GetTextureCount(type) <= 0)
		return {};

	std::vector<Texture> textures;
	textures.reserve(material->GetTextureCount(type));

	// TODO: should introduce some sort of cache to avoid reloading of textures very important

	for (int i = 0; i < material->GetTextureCount(type); i++) {
		aiString str;
		std::string directory = path.substr(0, path.find_last_of("/\\"));
		material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		std::string texPath = s_basePath + directory + "/" + str.C_Str();
		textures.push_back(Texture(texPath));
	}
}
