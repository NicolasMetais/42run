#pragma once
#include <iostream>
#include <optional>
#include "Matrix/Vector.hpp"
#include <fstream>
#include <Texture.hpp>
#include <sstream>
#include <unordered_map>


class Material {
	private:
		Vector<float> Ka;
		Vector<float> Kd;
		Vector<float> Ks;
		float Ns;
		float Ni;
		float d;
		int illum;

		std::optional<Texture> map_Ka;
		std::optional<Texture> map_Kd;
		std::optional<Texture> map_Ks;
		std::optional<Texture> map_Ns;
		std::optional<Texture> map_d;
		std::optional<Texture> bump;
		std::unordered_map<std::string, std::optional<Texture>*> textureMap;
		std::string name;
	public:
		Material();
		void setName(const std::string& name) { this->name = name;}
		void setKa(const Vector<float>& vec) { this->Ka = vec;}
		void setKd(const Vector<float>& vec) { this->Kd = vec;}
		void setKs(const Vector<float>& vec) { this->Ks = vec;}
		void setNs(const float& val) { this->Ns = val;}
		void setNi(const float& val) { this->Ni = val;}
		void setd(const float& val) { this->d = val;}
		void setIllum(const int& val) { this->illum = val;}
		void setTexture(const std::string& path, Texture texture);
		// void setMap_bump(const unsigned int& val) { this->bump = val;}
		std::string getName() const { return this->name;}
		Vector<float> getKd() const { return this->Kd;}
		Vector<float> getKa() const { return this->Ka;}
		Vector<float> getKs() const { return this->Ks;}
		float getNs() const { return this->Ns;}
		float getNi() const { return this->Ni;}
		float getd() const { return this->d;}
		int getIllum() const { return this->illum;}
		const std::optional<Texture>& getMapKa() const { return this->map_Ka;}
		const std::optional<Texture>& getMapKd() const { return this->map_Kd;}
		const std::optional<Texture>& getMapKs() const { return this->map_Ks;}
		const std::optional<Texture>& getMapNs() const { return this->map_Ns;}
		const std::optional<Texture>& getMapd() const { return this->map_d;}
		const std::optional<Texture>& getMapBump() const { return this->bump;}





};