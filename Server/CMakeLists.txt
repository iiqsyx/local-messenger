cmake_minimum_required(VERSION 3.24.)
project(server)

file(GLOB project
	src/*
)

add_executable(server ${project})
