#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "cap.h"

static int vector_length = 0;
static int num_vectors = 0;
static double epsilon = 0;
static double* vectorsList;
static double* centroids;
static int k;
static int max_iter;
static int converged = 0;



double distance(double*, double* b);
int findMinDistance(double* vector);
void vectorSum(double* a, double* b);
void scaleDownVector(double* a, double* b, int n);
int check_convergence(double* a, double* b, int n);
void updateCentroids(int* nextClustersSize, double* nextClustersSum);
void kmeans_c(double* vectorsList_c, double* centroids_c, int k_c, int max_iter_c, int num_vectors_c, int vector_length_c, double epsilon_c);

double distance(double* a, double* b)
{
	int i;
	/*Returns the square of the distance between the 2 vectors a,b
	  that are represented as double arrays of length vectorLen*/
	double sumOfSquares = 0;
	for (i = 0; i < vector_length; i++)
	{
		sumOfSquares += (*a - *b) * (*a - *b);
		a++;
		b++;
	}
	return sumOfSquares;
}

int findMinDistance(double* vector)
{
	int i;
	/*Finds the closest vector to the input named vector that exists in centroids
	  Returns the index of this input*/
	double* currentVector = centroids;
	double minDistance = distance(currentVector, vector);
	int minDistanceIndex = 0;
	for (i = 0; i < k; i++)
	{
		double currentDistance = distance(currentVector, vector);
		if (currentDistance < minDistance)
		{
			minDistance = currentDistance;
			minDistanceIndex = i;
		}
		currentVector += vector_length;
	}
	return minDistanceIndex;
}

void vectorSum(double* a, double* b)
{
	int i;
	/*Assumes b and a are vectors
	  Calculates vector a + b and writes the answer in a*/
	for (i = 0; i < vector_length; i++)
	{
		*a += *b;
		a++;
		b++;
	}
}

void scaleDownVector(double* a, double* b, int n)
{
	int i;
	/*Assumes b and a are vectors
	  Calculates the vector b/n and writes the answer in a*/
	for (i = 0; i < vector_length; i++)
	{
		*a = *b/n;
		a++;
		b++;
	}
}

int check_convergence(double* a, double* b, int n){
	int i;
	double dist_centroids = 0;
	for (i = 0; i < vector_length; i++)
	{
		dist_centroids += (*a - *b/n) * (*a - *b/n);
		a++;
		b++;
	}
	if(dist_centroids < epsilon * epsilon){
		return 1;
	}
	return 0;
}


void updateCentroids(int* nextClustersSize, double* nextClustersSum)
{
	int i;
	int j;
	int centroid_converged;
	int currentVectorClusterIndex;
	double* currentVectorCluster;
	int curr_iteration_convergence;
	double* currentVector;

	for (j = 0; j < k * vector_length; j++) {
		 *(nextClustersSum + j) = 0;
    }
	for (j = 0; j < k; j++){
		 *(nextClustersSize + j) = 0;
	}
		
	currentVector = vectorsList;
	/*This holds the pointer to the vector we work in the current iteration*/

	for (i = 0; i < num_vectors; i++)
	{
		currentVectorClusterIndex = findMinDistance(currentVector);

		currentVectorCluster = nextClustersSum + vector_length *currentVectorClusterIndex;
		/*The pointer to where we store the sum of vectors of the cluster we just added this element to*/
		/*We wish to add currentVector to this vector*/
		vectorSum(currentVectorCluster, currentVector);

		(*(nextClustersSize + currentVectorClusterIndex))++;
		/*We add 1 to the count of the size of the cluster*/

		currentVector += vector_length;
	}

	/*Now we update the list of centroids*/
	curr_iteration_convergence = 1;
	for (i = 0; i < k; i++)
	{
		centroid_converged = check_convergence(centroids + i * vector_length, nextClustersSum + i * vector_length, *(nextClustersSize + i));
		scaleDownVector(centroids + i * vector_length, nextClustersSum + i * vector_length, *(nextClustersSize + i));
		curr_iteration_convergence *= centroid_converged;
	}
	converged = curr_iteration_convergence;	

}

void kmeans_c(double* vectorsList_c, double* centroids_c, int k_c, int max_iter_c, int num_vectors_c, int vector_length_c, double epsilon_c){
	int iteration = 0;
	double* nextClustersSum;
	int* nextClustersSize;

	nextClustersSum = (double*)malloc(k * sizeof(double)*vector_length);
	/*Used to keep track of the sum of the vectors we put in every cluster*/
	nextClustersSize = (int*)malloc(k * sizeof(int));
	/*Used to keep track of how many vactors we put in every cluster*/

	
	centroids = centroids_c;
	vectorsList = vectorsList_c;
	k = k_c;
	max_iter = max_iter_c;
	num_vectors = num_vectors_c;
	vector_length = vector_length_c;
	epsilon = epsilon_c;

	if(nextClustersSize == NULL || nextClustersSum == NULL){
		free(vectorsList);
		free(centroids);
		free(nextClustersSize);
		free(nextClustersSum);
	
		centroids = NULL; 
	}

	while(!converged){
		
		updateCentroids(nextClustersSize, nextClustersSum);
		iteration++;
		if(iteration >= max_iter){
			break;
		}
	}
    /*Free the allocated memory*/
	free(nextClustersSize);
	free(nextClustersSum);
    
	
}



