#include <vector>
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>  
#include <assimp\scene.h>
#include "Vertex.h"
#include "Primitive.h"
#include "PrimitiveFactory.h"
#include "Runnable.h"
#include "Graphics.h"
#include <assimp\cimport.h>
#include <assimp\material.h>
#include <assimp\ai_assert.h>
#include "StaticModel.h"
#include "StaticMesh.h"
#include "Light.h"
#include "SkinnedModel.h"
#include "SkinnedMesh.h"
#include <fstream>
#include "cAnimationController.h"
#include "ModelImporter.h"

//! Graphics Library namespace.
namespace GLib
{

//! Constructor.
ModelImporter::ModelImporter(PrimitiveFactory* primitiveFactory)
{
	mPrimtiveFactory = primitiveFactory;	
}

//! Cleanup.
ModelImporter::~ModelImporter()
{
	// Cleanup all the models.
	for(auto iter = mStaticModelMap.begin(); iter != mStaticModelMap.end(); iter++) 
		delete (*iter).second;

	for(auto iter = mSkinnedModelMap.begin(); iter != mSkinnedModelMap.end(); iter++) 
		delete (*iter).second;
}

//! Loads and returns a skinned model from a file.
SkinnedModel* ModelImporter::LoadSkinnedModel(string filename)
{
	// Is the model already loaded?
	if(mSkinnedModelMap.find(filename) != mSkinnedModelMap.end())
		return mSkinnedModelMap[filename];

	Assimp::Importer importer;
	mFilename =	filename;
	SkinnedModel* model = NULL;

	// Important! Makes sure that if the angle between two face normals is > 80 they are not smoothed together.
	// Since the angle between a cubes face normals is 90 the lighting looks very bad if we don't specify this.
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);	
	importer.SetPropertyInteger(AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE);

	// Load scene from the file.
	const aiScene* scene = importer.ReadFile(filename, 
		aiProcess_CalcTangentSpace | 
		aiProcess_Triangulate | 
		aiProcess_GenSmoothNormals | 
		aiProcess_SplitLargeMeshes | 
		aiProcess_ConvertToLeftHanded | 
		aiProcess_SortByPType);

	if(scene)
	{
		// Create the model that is getting filled out.
		model = new SkinnedModel();

		// Create the animator.
		SceneAnimator* animator = new SceneAnimator();
		animator->Init(scene);
		model->SetAnimator(animator);

		// Loop through all meshes.
		for(int j = 0; j < scene->mNumMeshes; j++)
		{
			aiMesh* assimpMesh = scene->mMeshes[j];

			// Calculate vertex weight and bone indices.
			vector<Weights> weights = CalculateWeights(assimpMesh, animator);

			vector<SkinnedVertex> vertices;
			vector<UINT> indices;

			// Add vertices to the vertex list.
			for(int i = 0; i < assimpMesh->mNumVertices; i++) 
			{
				aiVector3D v = assimpMesh->mVertices[i];
				aiVector3D n = assimpMesh->mNormals[i];
				aiVector3D t = aiVector3D(0, 0, 0);
				if(assimpMesh->HasTextureCoords(0))
					t = assimpMesh->mTextureCoords[0][i];

				n = n.Normalize();

				// Pos, normal and texture coordinates.
				SkinnedVertex vertex(v.x, v.y, v.z, n.x, n.y, n.z, 0, 0, 1, t.x, t.y);

				// Bone indices and weights.
				for(int k = 0; k < weights[i].boneIndices.size(); k++) 
					vertex.BoneIndices[k] = weights[i].boneIndices[k];

				vertex.Weights.x = weights[i].weights.size() >= 1 ? weights[i].weights[0] : 0;
				vertex.Weights.y = weights[i].weights.size() >= 2 ? weights[i].weights[1] : 0;
				vertex.Weights.z = weights[i].weights.size() >= 3 ? weights[i].weights[2] : 0;

				vertices.push_back(vertex);
			}

			// Add indices to the index list.
			for(int i = 0; i < assimpMesh->mNumFaces; i++) 
				for(int k = 0; k < assimpMesh->mFaces[i].mNumIndices; k++) 
					indices.push_back(assimpMesh->mFaces[i].mIndices[k]);

			// Get the path to the texture in the directory.
			aiString path;
			aiMaterial* material = scene->mMaterials[assimpMesh->mMaterialIndex];
			material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path);
			FindValidPath(&path);

			// Extract all the ambient, diffuse and specular colors.
			aiColor4D ambient, diffuse, specular;
			material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
				
			// Create the mesh and its primitive.
			SkinnedMesh* mesh = new SkinnedMesh();

			Primitive* primitive = new Primitive(GlobalApp::GetD3DDevice(), vertices, indices);
			mesh->SetPrimitive(primitive);
			mesh->SetVertices(vertices);
			mesh->SetIndices(indices);
			mPrimtiveFactory->AddPrimitive(path.C_Str(), primitive);

			// Replace .tga with .bmp [HACK].
			string texturePath = path.C_Str();
			int tgaPos = texturePath.find_first_of(".tga");
			if(tgaPos != string::npos) {
				texturePath.replace(texturePath.size()-4, 4, ".bmp");
				path = texturePath;
			}

			// Any texture?
			if(_stricmp(path.C_Str(), "") != 0)
				mesh->LoadTexture(path.C_Str());

			// Any normal map?
			aiString nmap;
			material->Get(AI_MATKEY_TEXTURE_HEIGHT(0), nmap);
			FindValidPath(&nmap);
			if(_stricmp(nmap.C_Str(), "") != 0)	
				mesh->SetNormalMap(GlobalApp::GetGraphics()->LoadTexture(nmap.C_Str()));

			// [NOTE] The material is set to white.
			mesh->SetMaterial(Material(Colors::White));
			//mesh->SetMaterial(Material(diffuse, diffuse, diffuse));

			model->SetFilename(filename);

			// Add the mesh to the model.
			model->AddMesh(mesh);
		}

		// Pre-calculate the bounding box.
		model->CalculateAABB();

		// Add the newly created mesh to the map and return it.
		mSkinnedModelMap[filename] = model;
		return mSkinnedModelMap[filename];
	}
	else {
		char buffer[246];
		sprintf(buffer, "Error loading model: %s", filename.c_str());
		MessageBox(0, buffer, "Error!", 0);
		mSkinnedModelMap[filename] = LoadSkinnedModel("models/box.obj");
		return mSkinnedModelMap[filename];
	}
}

//! Loads and returns a static model from a file.
StaticModel* ModelImporter::LoadStaticModel(string filename)
{
	// Is the model already loaded?
	if(mStaticModelMap.find(filename) != mStaticModelMap.end())
		return mStaticModelMap[filename];

	Assimp::Importer importer;
	mFilename =	filename;
	StaticModel* model = NULL;

	// Important! Makes sure that if the angle between two face normals is > 80 they are not smoothed together.
	// Since the angle between a cubes face normals is 90 the lighting looks very bad if we don't specify this.
	importer.SetPropertyFloat(AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f);	
	importer.SetPropertyInteger(AI_CONFIG_IMPORT_TER_MAKE_UVS, 1);
	importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE);

	// Load scene from the file.
	const aiScene* scene = importer.ReadFile(filename, 
		aiProcess_CalcTangentSpace		| 
		aiProcess_Triangulate			| 
		aiProcess_GenSmoothNormals		|
		aiProcess_SplitLargeMeshes		|
		aiProcess_ConvertToLeftHanded	|
		aiProcess_SortByPType);

	// Successfully loaded the scene.
	if(scene)
	{
		// Create the model that is getting filled out.
		model = new StaticModel();

		// Loop through all meshes.
		for(int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* assimpMesh = scene->mMeshes[i];
			vector<Vertex>	vertices;
			vector<UINT>	indices;

			// Add vertices to the vertex list.
			for(int i = 0; i < assimpMesh->mNumVertices; i++) 
			{
				aiVector3D v = assimpMesh->mVertices[i];
				aiVector3D n = assimpMesh->mNormals[i];
				aiVector3D t = aiVector3D(0, 0, 0);
				if(assimpMesh->HasTextureCoords(0))
					t = assimpMesh->mTextureCoords[0][i];

				n = n.Normalize();
				Vertex vertex(v.x, v.y, v.z, n.x, n.y, n.z, 0, 0, 0, t.x, t.y);
				vertices.push_back(vertex);
			}

			// Add indices to the index list.
			for(int i = 0; i < assimpMesh->mNumFaces; i++) 
				for(int j = 0; j < assimpMesh->mFaces[i].mNumIndices; j++) 
					indices.push_back(assimpMesh->mFaces[i].mIndices[j]);

			// Get the path to the texture in the directory.
			aiString path;
			aiMaterial* material = scene->mMaterials[assimpMesh->mMaterialIndex];
			material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), path);
			FindValidPath(&path);

			// Extract all the ambient, diffuse and specular colors.
			aiColor4D ambient, diffuse, specular;
			material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
			material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			material->Get(AI_MATKEY_COLOR_SPECULAR, specular);

			// Create the mesh and its primitive.
			StaticMesh* mesh = new StaticMesh();
			Primitive* primitive = new Primitive(GlobalApp::GetD3DDevice(), vertices, indices);
			mesh->SetPrimitive(primitive);
			mesh->SetVertices(vertices);
			mesh->SetIndices(indices);
			mPrimtiveFactory->AddPrimitive(path.C_Str(), primitive);

			// Any texture?
			if(_stricmp(path.C_Str(), "") != 0)
				mesh->LoadTexture(path.C_Str());

			// Any normal map?
			aiString nmap;
			material->Get(AI_MATKEY_TEXTURE_HEIGHT(0), nmap);
			FindValidPath(&nmap);
			if(_stricmp(nmap.C_Str(), "") != 0)	
				mesh->SetNormalMap(GlobalApp::GetGraphics()->LoadTexture(nmap.C_Str()));

			// [NOTE] The material is set to white.
			mesh->SetMaterial(Material(Colors::White)); // Was  before [NOTE]

			model->SetFilename(filename);

			// Add the mesh to the model.
			model->AddMesh(mesh);
		}

		// Add to the model map and return it.
		mStaticModelMap[filename] = model;
		return mStaticModelMap[filename];
	}
	else {
		char buffer[246];
		sprintf(buffer, "Error loading model: %s", filename.c_str());
		MessageBox(0, buffer, "Error!", 0);
		mStaticModelMap[filename] = LoadStaticModel("models/box.obj");
		return mStaticModelMap[filename];
	}
}

//! Calculates the bone weights for each vertex.
vector<Weights> ModelImporter::CalculateWeights(aiMesh* mesh, SceneAnimator* animator)
{
	vector<Weights> weights(mesh->mNumVertices);

	// Loop through all bones.
	for(int i = 0; i < mesh->mNumBones; i++)
	{
		// Loop through all the vertices the bone affects.
		for(int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
		{
			aiVertexWeight weight = mesh->mBones[i]->mWeights[j];

			// Get the bone index from the animator by it's name.
			int index = animator->GetBoneIndex(mesh->mBones[i]->mName.C_Str());
			weights[weight.mVertexId].boneIndices.push_back(index);	
			weights[weight.mVertexId].weights.push_back(mesh->mBones[i]->mWeights[j].mWeight);
		}
	}

	return weights;
}

//! Hax function... not mine.
int ModelImporter::FindValidPath(aiString* p_szString)
{
	ai_assert(NULL != p_szString);
	aiString pcpy = *p_szString;
	if ('*' ==  p_szString->data[0])	{
		// '*' as first character indicates an embedded file
		return 5;
	}

	// first check whether we can directly load the file
	FILE* pFile = fopen(p_szString->data,"rb");
	if (pFile)fclose(pFile);
	else
	{
		// check whether we can use the directory of  the asset as relative base
		char szTemp[MAX_PATH*2], tmp2[MAX_PATH*2];
		strcpy(szTemp, mFilename.c_str());
		strcpy(tmp2,szTemp);

		char* szData = p_szString->data;
		if (*szData == '\\' || *szData == '/')++szData;

		char* szEnd = strrchr(szTemp,'\\');
		if (!szEnd)
		{
			szEnd = strrchr(szTemp,'/');
			if (!szEnd)szEnd = szTemp;
		}
		szEnd++;
		*szEnd = 0;
		strcat(szEnd,szData);


		pFile = fopen(szTemp,"rb");
		if (!pFile)
		{
			// convert the string to lower case
			for (unsigned int i = 0;;++i)
			{
				if ('\0' == szTemp[i])break;
				szTemp[i] = (char)tolower(szTemp[i]);
			}

			if(TryLongerPath(szTemp,p_szString))return 1;
			*szEnd = 0;

			// search common sub directories
			strcat(szEnd,"tex\\");
			strcat(szEnd,szData);

			pFile = fopen(szTemp,"rb");
			if (!pFile)
			{
				if(TryLongerPath(szTemp,p_szString))return 1;

				*szEnd = 0;

				strcat(szEnd,"textures\\");
				strcat(szEnd,szData);

				pFile = fopen(szTemp,"rb");
				if (!pFile)
				{
					if(TryLongerPath(szTemp, p_szString))return 1;
				}

				// patch by mark sibly to look for textures files in the asset's base directory.
				const char *path=pcpy.data; 
				const char *p=strrchr( path,'/' ); 
				if( !p ) p=strrchr( path,'\\' ); 
				if( p ){ 
					char *q=strrchr( tmp2,'/' ); 
					if( !q ) q=strrchr( tmp2,'\\' ); 
					if( q ){ 
						strcpy( q+1,p+1 ); 
						if(pFile=fopen( tmp2,"r" ) ){ 
							fclose( pFile ); 
							strcpy(p_szString->data,tmp2);
							p_szString->length = strlen(tmp2);
							return 1;
						} 
					} 
				}
				return 0;
			}
		}
		fclose(pFile);

		// copy the result string back to the aiString
		const size_t iLen = strlen(szTemp);
		size_t iLen2 = iLen+1;
		iLen2 = iLen2 > MAXLEN ? MAXLEN : iLen2;
		memcpy(p_szString->data,szTemp,iLen2);
		p_szString->length = iLen;

	}
	return 1;
}

//! Hax function... not mine.
bool ModelImporter::TryLongerPath(char* szTemp,aiString* p_szString)
{
	char szTempB[MAX_PATH];
	strcpy(szTempB,szTemp);

	// go to the beginning of the file name
	char* szFile = strrchr(szTempB,'\\');
	if (!szFile)szFile = strrchr(szTempB,'/');

	char* szFile2 = szTemp + (szFile - szTempB)+1;
	szFile++;
	char* szExt = strrchr(szFile,'.');
	if (!szExt)return false;
	szExt++;
	*szFile = 0;

	strcat(szTempB,"*.*");
	const unsigned int iSize = (const unsigned int) ( szExt - 1 - szFile );

	HANDLE          h;
	WIN32_FIND_DATA info;

	// build a list of files
	h = FindFirstFile(szTempB, &info);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (!(strcmp(info.cFileName, ".") == 0 || strcmp(info.cFileName, "..") == 0))
			{
				char* szExtFound = strrchr(info.cFileName, '.');
				if (szExtFound)
				{
					++szExtFound;
					if (0 == _stricmp(szExtFound,szExt))
					{
						const unsigned int iSizeFound = (const unsigned int) ( 
							szExtFound - 1 - info.cFileName);

						for (unsigned int i = 0; i < iSizeFound;++i)
							info.cFileName[i] = (CHAR)tolower(info.cFileName[i]);

						if (0 == memcmp(info.cFileName,szFile2, min(iSizeFound,iSize)))
						{
							// we have it. Build the full path ...
							char* sz = strrchr(szTempB,'*');
							*(sz-2) = 0x0;

							strcat(szTempB,info.cFileName);

							// copy the result string back to the aiString
							const size_t iLen = strlen(szTempB);
							size_t iLen2 = iLen+1;
							iLen2 = iLen2 > MAXLEN ? MAXLEN : iLen2;
							memcpy(p_szString->data,szTempB,iLen2);
							p_szString->length = iLen;
							return true;
						}
					}
					// check whether the 8.3 DOS name is matching
					if (0 == _stricmp(info.cAlternateFileName,p_szString->data))
					{
						strcat(szTempB,info.cAlternateFileName);

						// copy the result string back to the aiString
						const size_t iLen = strlen(szTempB);
						size_t iLen2 = iLen+1;
						iLen2 = iLen2 > MAXLEN ? MAXLEN : iLen2;
						memcpy(p_szString->data,szTempB,iLen2);
						p_szString->length = iLen;
						return true;
					}
				}
			}
		} 
		while (FindNextFile(h, &info));

		FindClose(h);
	}
	return false;
}

}	// End of Graphics Library namespace.