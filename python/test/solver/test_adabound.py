# Copyright 2019,2020,2021 Sony Corporation.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import pytest
import nnabla.solvers as S
import numpy as np
from solver_test_utils import solver_tester, RefSolver
from nbla_test_utils import list_context

ctxs = list_context('AdaBound')


class RefAdaBound(RefSolver):

    def __init__(self, alpha, beta1, beta2, eps, final_lr, gamma):
        self.alpha = alpha
        self.init_alpha = alpha
        self.beta1 = beta1
        self.beta2 = beta2
        self.eps = eps
        self.final_lr = final_lr
        self.gamma = gamma
        self.m = {}
        self.v = {}
        self.t = {}

    def _set_state_impl(self, key, param):
        self.m[key] = np.zeros_like(param)
        self.v[key] = np.zeros_like(param)
        self.t[key] = 0

    def _update_impl(self, key, p, g):
        self.t[key] = min(self.t[key] + 1, np.iinfo(np.int32).max)
        _update_adabound(p, g, self.m[key], self.v[key], self.t[key],
                         self.alpha, self.init_alpha, self.beta1, self.beta2, self.eps, self.final_lr, self.gamma)


def _update_adabound(p, g, m, v, t, alpha, init_alpha, beta1, beta2, eps, final_lr, gamma):
    alpha_t = alpha * \
        np.sqrt(1. - beta2 ** t) / (1. - beta1 ** t)
    final_lr_ = final_lr * (alpha / init_alpha)
    m[...] = beta1 * m + (1 - beta1) * g
    v[...] = beta2 * v + (1 - beta2) * g * g
    denom = np.sqrt(v) + eps
    lb = final_lr_ * (1 - 1 / (gamma*t + 1))
    ub = final_lr_ * (1 + 1 / (gamma*t))
    eta = np.clip(alpha_t/denom, lb, ub)
    p[...] = p - eta * m


@pytest.mark.parametrize("ctx, solver_name", ctxs)
@pytest.mark.parametrize("decay", [1e-4])
@pytest.mark.parametrize("alpha", [1e-2, 1e-4])
@pytest.mark.parametrize("beta1, beta2", [(0.9, 0.999), (0.999, 0.9)])
@pytest.mark.parametrize("eps", [1e-8])
@pytest.mark.parametrize("final_lr", [0.1])
@pytest.mark.parametrize("gamma", [0.001])
@pytest.mark.parametrize("seed", [313])
def test_adabound(seed, alpha, beta1, beta2, eps, final_lr, gamma, decay, ctx, solver_name):
    rng = np.random.RandomState(seed)
    solver_tester(
        rng, S.AdaBound, RefAdaBound, [
            alpha, beta1, beta2, eps, final_lr, gamma], atol=1e-3,
        ctx=ctx, solver_name=solver_name)
