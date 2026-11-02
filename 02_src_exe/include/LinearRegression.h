// LinearRegression.h
#ifndef LINEARREGRESSION_H // include guard
#define LINEARREGRESSION_H
#endif
#pragma once

#include <iostream>
#include <vector>
#include <numeric> // For std::accumulate

//
class LinearRegression
{
	public : 
	   //	 
	   struct _DataPoint {
    		double x; // x
    		double y; // y (Total mission duration in hours)
	   };
       //
	   double slope                           = 0.0;
       double intercept                       = 0.0;
	   std::vector<_DataPoint> historicalData = {};
	   	
	   //	
	   LinearRegression()
	   {
		    // Historical Data: Mission Number vs. Total Mission Duration (Hours)
		    // Apollo 8: 6 days 3 hours     = (6 * 24) + 3 = 147 hours
		    // Apollo 10: 8 days 37 minutes = (8 * 24) + 37/60 ~ 192.6 hours -> Rounded to 193
		    // Apollo 11: 8 days 3 hours    = (8 * 24) + 3 = 195 hours
		    // Apollo 12: 10 days 4 hours   = (10 * 24) + 4 = 244 hours
		    // Apollo 13: 5 days 22 hours   = (5 * 24) + 22 = 142 hours
		    // Apollo 14: 9 days 1 hour     = (9 * 24) + 1 = 217 hours
		    // Apollo 15: 12 days 7 hours   = (12 * 24) + 7 = 295 hours
		    // Apollo 16: 11 days 1 hour    = (11 * 24) + 1 = 265 hours
		    // Apollo 17: 12 days 13 hours  = (12 * 24) + 13 = 301 hours
	   		this->historicalData = {
		        {8.0,  147.0}, // Apollo 8  // Apollo 9 did not reach the moon
		        {10.0, 193.0}, // Apollo 10 (Corrected)
		        {11.0, 195.0}, // Apollo 11
		        {12.0, 244.0}, // Apollo 12
		        {13.0, 142.0}, // Apollo 13
		        {14.0, 217.0}, // Apollo 14
		        {15.0, 295.0}, // Apollo 15
		        {16.0, 265.0}, // Apollo 16
		        {17.0, 301.0}  // Apollo 17
    		};
    		
    		// Perform linear regression to find the best-fit line for Mission Number vs Total Mission Time
    		this->simpleLinearRegression(this->historicalData, this->slope, this->intercept);
	   };
	   
	   // Function to perform Simple Linear Regression using Least Squares Method
	   void simpleLinearRegression(const std::vector<_DataPoint>& data, double& slope, double& intercept) 
	   {
		    int n = data.size();
		    if (n == 0) {
		        slope     = 0.0;
		        intercept = 0.0;
		        return;
		    }
		
		    // Calculate means of x and y
		    double sumX = 0.0, sumY = 0.0;
		    for (const auto& point : data) {
		        sumX += point.x;
		        sumY += point.y;
		    }
		    double meanX = sumX / n;
		    double meanY = sumY / n;
		
		    // Calculate the slope (m) and intercept (b)
		    // Formula for slope: m = sum((x - meanX) * (y - meanY)) / sum((x - meanX)^2)
		    double numerator   = 0.0;
		    double denominator = 0.0;
		
		    for (const auto& point : data) {
		        double x_deviation = point.x - meanX;
		        double y_deviation = point.y - meanY;
		        numerator   += x_deviation * y_deviation;
		        denominator += x_deviation * x_deviation;
		    }
		
		    if (denominator != 0) {
		        slope = numerator / denominator;
		    } else {
		        slope = 0.0; // Avoid division by zero if all x values are the same
		    }
		
		    // Formula for intercept: b = meanY - m * meanX
		    intercept = meanY - slope * meanX;
		}
		
		//
		double predict(double missionNumberToPredict)
		{
			
			// Perform linear regression to find the best-fit line for Mission Number vs Total Mission Time
    		this->simpleLinearRegression(this->historicalData, this->slope, this->intercept);
    		
    		// Note: This prediction is based on a very simplistic model applied to limited and potentially non-linear data.
		    // It does not represent a realistic prediction for an actual mission.
    		double predictedTotalTime     = slope * missionNumberToPredict + intercept;

   			// std::cout << "\nPredicted total mission time for Apollo " << static_cast<int>(missionNumberToPredict + 8) << ": " << predictedTotalTime << " hours. " << ((predictedTotalTime)/24) << " days. "<< std::endl;
		
            return predictedTotalTime;
 		}	
};
