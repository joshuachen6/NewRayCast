#include "CastUtils.h"
#include "ResourceManager.h"

CastResult::CastResult(TexturedVertex vertex, double cx, double cy, double distance) {
	this->vertex = vertex;
	this->cx = cx;
	this->cy = cy;
	this->distance = distance;
}

vector<CastResult> castRay(double angle, double FOV, Vector3f player) {
	double slope = tan(angle);
	double b = -slope * player.x + player.y;

	vector<CastResult> hits;

	for (TexturedVertex vertex : ResourceManager::vertecies) {
		double cx, cy;
		if (vertex.start.x != vertex.end.x) {
			double vslope = (vertex.end.y - vertex.start.y) / (vertex.end.x - vertex.start.x);
			bool collides = (slope - vslope) != 0;
			if (!collides) {
				continue;
			}
			double vb = -vslope * vertex.start.x + vertex.start.y;
			cx = (vb - b) / (slope - vslope);
			cy = slope * cx + b;
		} else {
			cy = slope * vertex.start.x + b;
			cx = vertex.start.x;
		}

		bool yrange = (cy - min(vertex.start.y, vertex.end.y)) > -0.5 && (cy - max(vertex.start.y, vertex.end.y)) < 0.5;
		bool xrange = (cx - min(vertex.start.x, vertex.end.x)) > -0.5 && (cx - max(vertex.start.x, vertex.end.x)) < 0.5;

		if (!(yrange && xrange)) {
			continue;
		}

		if (abs(scaleAngle((atan2((cy - player.y), (cx - player.x)) - player.z))) > FOV / 2) {
			continue;
		}

		double distance = sqrt(pow(cx - player.x, 2) + pow(cy - player.y, 2));
		hits.push_back(CastResult(vertex, cx, cy, distance));
	}

	sort(hits.begin(), hits.end(), [](CastResult a, CastResult b) {
		return a.distance < b.distance;
		});

	return hits;
}

double scaleAngle(double angle) {
	while (angle > PI) {
		angle -= 2 * PI;
	}

	while (angle < -PI) {
		angle += 2 * PI;
	}

	return angle;
}
