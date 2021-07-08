#include "Fluid.h"
#include <iostream>

Fluid::Fluid(int n, float r) {
  num_particles = n;
  kernel_radius = r;
  rest_density = 0.05;
  fluid_constant = 10000;

  for (int i = 0; i < num_particles; i++) {
    Particle initializer = Particle((i / 60) * 1.8 + 40, (i % 60) * 1.8 + 10,
                                    0.5, 0, rest_density, fluid_constant);
    Particles.push_back(initializer);
  }

  for (int i = 0; i < num_particles; i++) {
    addresses.push_back(&Particles[i]);
  }

  optimize = new SpatialGrid(200, 200, kernel_radius);

  forces = new SPHKernels(kernel_radius);
}

void Fluid::update(float dt) {
  optimize->hash(addresses);
  optimize->sort(addresses);
  optimize->point(addresses);
  calculatePressure();
  calculateForces();
  color();
  integrate(dt);
  walls(dt);
}

void Fluid::mousePush(float x, float y, float x_f, float y_f) {
  ex_x = x;
  ex_y = y;
  ex_force_x = x_f;
  ex_force_y = y_f;
}

void Fluid::calculatePressure() {
  for (int i = 0; i < num_particles; i++)
    addresses[i]->col = sf::Color::White;

  Particle** test = &addresses[0];
  for (int i = 0; i < num_particles; i++) {
    int address = i;
    int address_increment;

    // interactions with particles in the same index
    address_increment = 0;
    while (num_particles > address + address_increment &&
           addresses[address]->getIndex() ==
               addresses[address + address_increment]->getIndex()) {
      if (i + address_increment < num_particles) {
        forces->density(addresses[i], addresses[i + address_increment]);
        address_increment++;
      }
    }
    // interactions with particles in neighboring indexe
    if (optimize->hasBottomLeft(addresses[i]->getIndex())) {
      address = optimize->getBottomLeftPointer(addresses[i]->getIndex());
      address_increment = 0;
      while (num_particles > address + address_increment &&
             addresses[address]->getIndex() ==
                 addresses[address + address_increment]->getIndex()) {
        forces->density(addresses[i], addresses[address + address_increment]);
        address_increment++;
      }
    }

    if (optimize->hasBottom(addresses[i]->getIndex())) {
      address = optimize->getBottomPointer(addresses[i]->getIndex());
      address_increment = 0;
      while (num_particles > address + address_increment &&
             addresses[address]->getIndex() ==
                 addresses[address + address_increment]->getIndex()) {
        forces->density(addresses[i], addresses[address + address_increment]);
        address_increment++;
      }
    }

    if (optimize->hasBottomRight(addresses[i]->getIndex())) {
      address = optimize->getBottomRightPointer(addresses[i]->getIndex());
      address_increment = 0;
      while (num_particles > address + address_increment &&
             addresses[address]->getIndex() ==
                 addresses[address + address_increment]->getIndex()) {
        forces->density(addresses[i], addresses[address + address_increment]);
        address_increment++;
      }
    }

    if (optimize->hasRight(addresses[i]->getIndex())) {
      address = optimize->getRightPointer(addresses[i]->getIndex());
      address_increment = 0;
      while (num_particles > address + address_increment &&
             addresses[address]->getIndex() ==
                 addresses[address + address_increment]->getIndex()) {
        forces->density(addresses[i], addresses[address + address_increment]);
        address_increment++;
      }
    }
    addresses[i]->setPressure();
  }
}

void Fluid::calculateForces() {
  for (int i = 0; i < num_particles; i++) {
    int address = i;
    int address_increment;

    // interactions with particles in the same index
    address_increment = 1;
    while (num_particles > address + address_increment &&
           addresses[i]->getIndex() ==
               addresses[i + address_increment]->getIndex()) {
      if (i + address_increment < num_particles) {
        forces->pressure(addresses[i], addresses[i + address_increment]);
        forces->viscosity(addresses[i], addresses[i + address_increment]);
        forces->surface_tension(addresses[i], addresses[i + address_increment]);
        address_increment++;
      }
    }
    // interactions with particles
    if (optimize->hasBottomLeft(addresses[i]->getIndex())) {
      address = optimize->getBottomLeftPointer(addresses[i]->getIndex());
      address_increment = 0;
      while (num_particles > address + address_increment &&
             addresses[address]->getIndex() ==
                 addresses[address + address_increment]->getIndex()) {
        forces->pressure(addresses[i], addresses[address + address_increment]);
        forces->viscosity(addresses[i], addresses[address + address_increment]);
        forces->surface_tension(addresses[i],
                                addresses[address + address_increment]);
        address_increment++;
      }
    }

    if (optimize->hasBottom(addresses[i]->getIndex())) {
      address = optimize->getBottomPointer(addresses[i]->getIndex());
      address_increment = 0;
      while (num_particles > address + address_increment &&
             addresses[address]->getIndex() ==
                 addresses[address + address_increment]->getIndex()) {
        forces->pressure(addresses[i], addresses[address + address_increment]);
        forces->viscosity(addresses[i], addresses[address + address_increment]);
        forces->surface_tension(addresses[i],
                                addresses[address + address_increment]);
        address_increment++;
      }
    }

    if (optimize->hasBottomRight(addresses[i]->getIndex())) {
      address = optimize->getBottomRightPointer(addresses[i]->getIndex());
      address_increment = 0;
      while (num_particles > address + address_increment &&
             addresses[address]->getIndex() ==
                 addresses[address + address_increment]->getIndex()) {
        forces->pressure(addresses[i], addresses[address + address_increment]);
        forces->viscosity(addresses[i], addresses[address + address_increment]);
        forces->surface_tension(addresses[i],
                                addresses[address + address_increment]);
        address_increment++;
      }
    }

    if (optimize->hasRight(addresses[i]->getIndex())) {
      address = optimize->getRightPointer(addresses[i]->getIndex());
      address_increment = 0;
      while (num_particles > address + address_increment &&
             addresses[address]->getIndex() ==
                 addresses[address + address_increment]->getIndex()) {
        forces->pressure(addresses[i], addresses[address + address_increment]);
        forces->viscosity(addresses[i], addresses[address + address_increment]);
        forces->surface_tension(addresses[i],
                                addresses[address + address_increment]);
        address_increment++;
      }
    }
    addresses[i]->addForce(0, addresses[i]->getDensity() * 12);
    int distance_sq = pow((Particles[i].getX() - ex_x), 2) +
                      pow((Particles[i].getY() - ex_y), 2);
    if (distance_sq < 3 * pow(kernel_radius, 2)) {
      Particles[i].addForce(ex_force_x, ex_force_y);
    }
  }
}

void Fluid::color() {
  for (int i = 0; i < num_particles; i++) {
    Particles[i].col = sf::Color(100, 200, 255, 150);
  }
}

void Fluid::integrate(float dt) {
  for (int i = 0; i < num_particles; i++) {
    Particles[i].update(dt);
  }
}

void Fluid::walls(float dt) {
  for (int i = 0; i < num_particles; i++) {
    if (Particles[i].getX() <= 1) {
      Particles[i].setX(1.1);
      Particles[i].setU(Particles[i].getU() * -0.5);
    }
    if (Particles[i].getX() >= 149) {
      Particles[i].setX(148.9);
      Particles[i].setU(Particles[i].getU() * -0.5);
    }
    if (Particles[i].getY() <= 1) {
      Particles[i].setY(1.1);
      Particles[i].setV(Particles[i].getV() * -0.5);
    }
    if (Particles[i].getY() >= 189) {
      Particles[i].setY(188.9);
      Particles[i].setV(Particles[i].getV() * -0.5);
    }
  }
}

void Fluid::getCoordinates(std::vector<float>& x, std::vector<float>& y) {
  for (int i = 0; i < num_particles; i++) {
    x[i] = Particles[i].getX();
    y[i] = Particles[i].getY();
  }
}

void Fluid::getCol(std::vector<sf::Color>& col) {
  for (int i = 0; i < num_particles; i++) {
    col[i] = Particles[i].col;
  }
}

Fluid::~Fluid() {}
