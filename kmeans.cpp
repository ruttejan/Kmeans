#include "kmeans.hpp"

PointKmeans::PointKmeans(double x, double y)
{
	this->x = x;
	this->y = y;
}

bool PointKmeans::equal(PointKmeans& p) {
	double diff = abs(this->x - p.getX()) + abs(this->y - p.getY());
	return diff < 0.0001;
}

Kmeans::Kmeans(vector<PointKmeans> points, size_t k, size_t maxIter)
{
	this->points = points;
	this->k = k;
	this->maxIter = maxIter;
	this->centroids = vector<PointKmeans>();
}

// Euclidian distance without the square root
double squaredEuclidianDist(PointKmeans p1, PointKmeans p2){
	return pow(p1.getX() - p2.getX(), 2.0) + pow(p1.getY() - p2.getY(), 2.0);
}

void Kmeans::initializeCentroids()
{
	
	if (this->points.empty()) {
		cout << "No points to initialize centroids." << endl;
		return;
	}


	this->centroids = vector<PointKmeans>(this->k);

	// shuffle the points
	vector<size_t> indices = vector<size_t>(this->points.size());
	iota(indices.begin(), indices.end(), 0);
	static mt19937 mt{random_device{}()};
	shuffle(indices.begin(), indices.end(), mt);

	// take k first elements in the shuffled array
	for (size_t i = 0; i < this->k; i++)
	{
		this->centroids[i] = this->points[indices[i]];
	}

}

vector<vector<PointKmeans>> Kmeans::initializeCentroidsForMultipleTrials(size_t nTrials)
{
	vector<vector<PointKmeans>> initCentroids(nTrials);

	for (size_t i = 0; i < nTrials; i++)
	{
		// shuffle the points
		vector<size_t> indices = vector<size_t>(this->points.size());
		iota(indices.begin(), indices.end(), 0);
		static mt19937 mt{random_device{}()};
		shuffle(indices.begin(), indices.end(), mt);

		// take k first elements in the shuffled array
		vector<PointKmeans> centroids(this->k);
		for (size_t j = 0; j < this->k; j++)
		{
			centroids[j] = this->points[indices[j]];
		}

		initCentroids[i] = centroids;
	}

	return initCentroids;
}

pair<vector<PointKmeans>, vector<vector<PointKmeans>>> Kmeans::k_means()
{

	// initialize Centroids from given points
	if (this->centroids.empty())
		this->initializeCentroids();

	bool converged = true;

	for (size_t i = 0; i < this->maxIter; i++)
	{
		sqDist = 0.0;
		vector<vector<PointKmeans>> clusters = vector<vector<PointKmeans>>(this->k, vector<PointKmeans>());
		vector<PointKmeans> newCentroids = vector<PointKmeans>(this->k);

		// assign each point to a cluster
		for (PointKmeans &point : this->points)
		{

			double min = numeric_limits<double>::max();
			size_t minIdx = 0;

			// compute distance to each centroid and select the minimal one
			for (size_t j = 0; j < this->k; j++)
			{
				double dist = squaredEuclidianDist(point, this->centroids[j]);
				if (dist < min)
				{
					sqDist += dist;
					min = dist;
					minIdx = j;
				}
			}
			clusters[minIdx].push_back(point);
		}

		// calculate new centroids - calculate mean for each cluster
		// and check if the new centroids are same as the previous centroids
		for (size_t j = 0; j < this->k; j++)
		{

			double sumX = 0.0;
			double sumY = 0.0;

			for (auto p : clusters[j])
			{
				sumX += p.getX();
				sumY += p.getY();
			}

			// compute the mean of all points in a cluster
			double meanX = sumX / clusters[j].size();
			double meanY = sumY / clusters[j].size();
			
			newCentroids[j] = PointKmeans(meanX, meanY);
			double diff = abs(newCentroids[j].getX() - this->centroids[j].getX()) + abs(newCentroids[j].getY() - this->centroids[j].getY());
			if (diff > 0.0001)
				converged = false;
		}

		if (converged)
		{
			return {newCentroids, clusters};
		}
		else {
			converged = true;
			this->centroids = newCentroids;
		}
	}
	if(!converged)
		cout << "Did not converge." << endl;
		return {vector<PointKmeans>(), vector<vector<PointKmeans>>()};
}

tuple<double, vector<PointKmeans>, vector<vector<PointKmeans>>> Kmeans::k_meansForParallel(vector<PointKmeans> c)
{
	double minSqDist;
	vector<PointKmeans> finalCentroids;
	vector<vector<PointKmeans>> finalClusters;

	bool converged = true;

	for (size_t i = 0; i < this->maxIter; i++)
	{
		minSqDist = 0.0;
		vector<vector<PointKmeans>> clusters = vector<vector<PointKmeans>>(this->k, vector<PointKmeans>());
		vector<PointKmeans> newCentroids = vector<PointKmeans>(this->k);

		// assign each point to a cluster
		for (PointKmeans &point : this->points)
		{

			double min = numeric_limits<double>::max();
			size_t minIdx = 0;

			// compute distance to each centroid and select the minimal one
			for (size_t j = 0; j < this->k; j++)
			{
				double dist = squaredEuclidianDist(point, c[j]);
				if (dist < min)
				{
					minSqDist += dist;
					min = dist;
					minIdx = j;
				}
			}

			clusters[minIdx].push_back(point);
		}

		// calculate new centroids - calculate mean for each cluster
		// and check if the new centroids are same as the previous centroids
		for (size_t j = 0; j < this->k; j++)
		{

			double sumX = 0.0;
			double sumY = 0.0;

			for (auto p : clusters[j])
			{
				sumX += p.getX();
				sumY += p.getY();
			}

			// compute the mean of all points in a cluster
			double meanX = sumX / clusters[j].size();
			double meanY = sumY / clusters[j].size();
			
			newCentroids[j] = PointKmeans(meanX, meanY);
			double diff = abs(newCentroids[j].getX() - c[j].getX()) + abs(newCentroids[j].getY() - c[j].getY());
			if (diff > 0.0001)
				converged = false;
		}

		if (converged)
		{
			finalCentroids = newCentroids;
			finalClusters = clusters;
			break;
		}
		else {
			converged = true; // set converged to default value
			c = newCentroids;
		}
	}
	if(!converged){
		minSqDist = numeric_limits<double>::max();
		finalCentroids = vector<PointKmeans>();
		finalClusters = vector<vector<PointKmeans>>();
		cout << "Did not converge." << endl;
	}

	return {minSqDist, finalCentroids, finalClusters};
}

pair<vector<PointKmeans>, vector<vector<PointKmeans>>> Kmeans::k_meansMultipleTrials(size_t nTrials, vector<vector<PointKmeans>> initCentroids){

	double minSqDist = numeric_limits<double>::max();
	vector<PointKmeans> bestCentroids;
	vector<vector<PointKmeans>> finalClusters;

	// run all trials of kmeans
	for (size_t i = 0; i < nTrials; i++)
	{
		this->centroids.clear();
		this->centroids = initCentroids[i];
		auto res = this->k_means();
		// select the best solution
		if (this->sqDist < minSqDist)
		{
			minSqDist = this->sqDist;
			bestCentroids = res.first;
			finalClusters = res.second;
		}
	}
	return {bestCentroids, finalClusters};
}

pair<vector<PointKmeans>, vector<vector<PointKmeans>>> Kmeans::k_meansParallelMultipleTrials(size_t nTrials, vector<vector<PointKmeans>> initCentroids)
{
	double minSqDist = numeric_limits<double>::max();
    vector<PointKmeans> bestCentroids;
	vector<vector<PointKmeans>> finalClusters;

    // Number of threads to use
    size_t numThreads = (thread::hardware_concurrency() < nTrials) ? thread::hardware_concurrency() : nTrials;

    vector<thread> threads(numThreads);
    vector< tuple< double, vector<PointKmeans>, vector<vector<PointKmeans>>>> results(nTrials);

	// function for running a single trial
    auto trialFunction = [this, &results, &initCentroids](size_t trial_index) {
        this->centroids.clear();
        auto res = this->k_meansForParallel(initCentroids[trial_index]);
        results[trial_index] = make_tuple(get<0>(res), get<1>(res), get<2>(res));
    };

    // Launch the threads for each trial
    for (size_t i = 0; i < nTrials; i++)
    {
		// if there are more centroids than threads - reuse the threads
        if (i >= numThreads)
        {
            threads[i % numThreads].join();
        }

        threads[i % numThreads] = thread(trialFunction, i);
    }

    // Ensure all threads are joined before proceeding
    for (size_t i = 0; i < numThreads; ++i)
    {
		// Check if the thread is joinable before joining
        if (threads[i].joinable())  
        {
            threads[i].join();
        }
    }

    // Find the best output
    for (const auto& result : results)
    {
        if (get<0>(result) < minSqDist)
        {
            minSqDist = get<0>(result);
            bestCentroids = get<1>(result);
			finalClusters = get<2>(result);
        }
    }

    return {bestCentroids, finalClusters};
}





ParallelKmeans::ParallelKmeans(vector<PointKmeans> data, size_t k, vector<PointKmeans> centroids, size_t maxIter)
: Kmeans(data, k, maxIter)
{
	this->centroids = centroids;
}

pair<vector<PointKmeans>, vector<vector<PointKmeans>>> ParallelKmeans::k_means()
{
	size_t numThreads = thread::hardware_concurrency();
    atomic<bool> converged(true);

    for (size_t iter = 0; iter < this->maxIter; iter++)
    {
		vector<thread> threads(numThreads);
        vector<vector<PointKmeans>> clusters(this->k);
        mutex clusterMutex;

        vector<PointKmeans> newCentroids(this->k);

        // Parallel point assignment to each cluster
        for (size_t t = 0; t < numThreads; ++t)
        {
            size_t pointsPerThread = this->points.size() / numThreads;
            size_t start = t * pointsPerThread;
            size_t end = (t == numThreads - 1) ? this->points.size() : start + pointsPerThread;

            threads[t] = thread([this, start, end, &clusters, &clusterMutex, t]() {
                for (size_t i = start; i < end; ++i)
                {
                    PointKmeans& point = this->points[i];
                    double minDist = numeric_limits<double>::max();
                    size_t bestCluster = 0;

					// compute distance to each centroid and select the minimal one
                    for (size_t j = 0; j < this->k; ++j)
                    {
						double dist = squaredEuclidianDist(point, this->centroids[j]);
                        if (dist < minDist)
                        {
                            minDist = dist;
                            bestCluster = j;
                        }
                    }

                    lock_guard<mutex> lock(clusterMutex);
                    clusters[bestCluster].push_back(point);
                }
            });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }


        // Parallel computation of new centroids
        threads.resize(numThreads);
        converged.store(true, std::memory_order_relaxed);

        for (size_t t = 0; t < numThreads; ++t)
        {
            threads[t] = thread([t, numThreads, &clusters, &newCentroids, &converged, this]() {
                for (size_t j = t; j < this->k; j += numThreads)
                {
                    double sumX = 0.0;
                    double sumY = 0.0;

                    for (auto& p : clusters[j])
                    {
                        sumX += p.getX();
                        sumY += p.getY();
                    }

					// compute the mean of all points in a cluster
                    if (!clusters[j].empty())
                    {
                        double meanX = sumX / clusters[j].size();
                        double meanY = sumY / clusters[j].size();
                        newCentroids[j] = PointKmeans(meanX, meanY);

                        double diff = abs(newCentroids[j].getX() - this->centroids[j].getX()) +
                                      abs(newCentroids[j].getY() - this->centroids[j].getY());
                        if (diff > 0.0001)
                        {
                            converged.store(false, std::memory_order_relaxed);
                        }
                    }
                    else
                    {
                        newCentroids[j] = this->centroids[j];
                    }
                }
            });
        }

        for (auto& thread : threads)
        {
            thread.join();
        }

        // Check for convergence
        if (converged.load(std::memory_order_relaxed))
        {
			return {newCentroids, clusters};
        }
        else
        {
            this->centroids = newCentroids;
        }
    }

    if (!converged.load(std::memory_order_relaxed))
        cout << "Parallel did not converge." << endl;
		return {vector<PointKmeans>(), vector<vector<PointKmeans>>()};
}



int getRandomIndex(size_t n)
{
	static mt19937 mt{random_device{}()};
	static uniform_int_distribution<> dist(0, n);
	return dist(mt);
}

void KmeansPlusPlus::initializeCentroids()  {
	if (this->points.empty()) return;

	this->centroids = vector<PointKmeans>(this->k);

	// select first centroid from the points
	this->centroids[0] = this->points[getRandomIndex(this->points.size() - 1)];

	for (size_t i = 1; i < this->k; ++i) {
		vector<double> distances(this->points.size(), numeric_limits<double>::max());

		// count weights of the points (distance to the closest centroid)
		for (size_t j = 0; j < this->points.size(); ++j) {
			for (size_t m = 0; m < i; ++m) {
				PointKmeans point = this->points[j];
				PointKmeans centroid = this->centroids[m];
				double dist = squaredEuclidianDist(point, centroid);
				distances[j] = min(distances[j], dist);
			}
		}

		// select a point from the wighted probability distribution
		double totalDistance = accumulate(distances.begin(), distances.end(), 0.0);
		uniform_real_distribution<> distribution(0, totalDistance);
		double randomValue = distribution(mt19937(random_device{}()));

		double cumulative = 0.0;
		for (size_t j = 0; j < distances.size(); ++j) {
			cumulative += distances[j];
			if (cumulative >= randomValue) {
				this->centroids[i] = this->points[j];
				break;
			}
		}
	}
}
